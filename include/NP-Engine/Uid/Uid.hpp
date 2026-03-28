//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/31/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_UID_HPP
#define NP_ENGINE_UID_HPP

#define NP_ENGINE_UID_HANDLE_INVALID_KEY 0
#define NP_ENGINE_UID_HANDLE_INVALID_GENERATION 0

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Random/Random.hpp"

#include "NP-Engine/Vendor/StduuidInclude.hpp"

namespace np::uid
{
	using Uid = ::uuids::uuid;

	struct UidHandle
	{
		using KeyType = ui32;
		using GenerationType = ui32;

		KeyType key = NP_ENGINE_UID_HANDLE_INVALID_KEY;
		GenerationType generation = NP_ENGINE_UID_HANDLE_INVALID_GENERATION;

		bl IsValid() const
		{
			return key != NP_ENGINE_UID_HANDLE_INVALID_KEY && generation != NP_ENGINE_UID_HANDLE_INVALID_GENERATION;
		}

		operator bl() const
		{
			return IsValid();
		}

		void Invalidate()
		{
			key = NP_ENGINE_UID_HANDLE_INVALID_KEY;
			generation = NP_ENGINE_UID_HANDLE_INVALID_GENERATION;
		}
	};

	class UidSystem
	{
	private:
		class HndlDestroyer : public mem::smart_ptr_contiguous_destroyer<UidHandle>
		{
		private:
			using base = mem::smart_ptr_contiguous_destroyer<UidHandle>;
			UidSystem& _uid_system;

		public:
			HndlDestroyer(UidSystem& uid_system, mem::allocator& a): base(a), _uid_system(uid_system) {}

			HndlDestroyer(const HndlDestroyer& other): base(other), _uid_system(other._uid_system) {}

			HndlDestroyer(HndlDestroyer&& other) noexcept: base(other), _uid_system(other._uid_system) {}

			void destruct_object(UidHandle* ptr) override
			{
				_uid_system.ReleaseHandle(*ptr);
				base::destruct_object(ptr);
			}
		};

		using HndlResource = mem::smart_ptr_resource<UidHandle, HndlDestroyer>;
		using HndlBlock = mem::smart_ptr_contiguous_block<UidHandle, HndlResource>;

		class UidGenerator : public uuids::basic_uuid_random_generator<rng::Random64>
		{
		private:
			rng::Random64 _rng;

		public:
			UidGenerator(): uuids::basic_uuid_random_generator<rng::Random64>(_rng) {}

			rng::Random64& GetRng()
			{
				return _rng;
			}

			const rng::Random64& GetRng() const
			{
				return _rng;
			}
		};

		struct UidRecord
		{
			mem::sptr<Uid> uidSptr = nullptr;
			UidHandle::GenerationType generation = NP_ENGINE_UID_HANDLE_INVALID_GENERATION;

			bl IsValid() const
			{
				return uidSptr && generation != NP_ENGINE_UID_HANDLE_INVALID_GENERATION;
			}

			operator bl() const
			{
				return IsValid();
			}

			void Invalidate()
			{
				uidSptr.reset();
				generation = NP_ENGINE_UID_HANDLE_INVALID_GENERATION;
			}
		};

		struct UidBookKeeping
		{
			UidHandle nextHandle;
			UidGenerator generator;
			con::uset<UidHandle::KeyType> releasedKeys;
			con::umap<UidHandle::KeyType, UidRecord> keyToRecord;
			con::uset<Uid> masterSet;
		};

		mem::trait_allocator _allocator;
		const HndlDestroyer _destroyer;
		mem::accumulating_pool<Uid, mem::DEFAULT_ALIGNMENT> _pool;
		mutexed_wrapper<UidBookKeeping> _bookkeeping;

		static UidHandle::KeyType GetNextKey(UidBookKeeping& bookkeeping)
		{
			UidHandle::KeyType k = NP_ENGINE_UID_HANDLE_INVALID_KEY;

			if (bookkeeping.releasedKeys.size())
			{
				k = *bookkeeping.releasedKeys.begin();
				bookkeeping.releasedKeys.erase(bookkeeping.releasedKeys.begin());
			}
			else
			{
				do
					++bookkeeping.nextHandle.key;
				while (bookkeeping.nextHandle.key == NP_ENGINE_UID_HANDLE_INVALID_KEY);
				k = bookkeeping.nextHandle.key;
			}

			return k;
		}

		static UidHandle::GenerationType GetNextGeneration(UidBookKeeping& bookkeeping)
		{
			do
				++bookkeeping.nextHandle.generation;
			while (bookkeeping.nextHandle.generation == NP_ENGINE_UID_HANDLE_INVALID_GENERATION);
			return bookkeeping.nextHandle.generation;
		}

		void ReleaseHandle(UidHandle& hndl)
		{
			if (hndl.IsValid())
			{
				auto bookkeeping = _bookkeeping.get_access();
				auto it = bookkeeping->keyToRecord.find(hndl.key);
				if (it != bookkeeping->keyToRecord.end())
				{
					UidRecord record = it->second;
					if (hndl.generation == record.generation && _pool.contains(record.uidSptr))
					{
						bookkeeping->keyToRecord.erase(it);
						bookkeeping->releasedKeys.emplace(hndl.key);
						bookkeeping->masterSet.erase(*record.uidSptr);
						record.Invalidate();
						hndl.Invalidate();
					}
				}
			}
		}

	public:
		UidSystem(): _destroyer(*this, _allocator) {}

		~UidSystem()
		{
			Dispose();
		}

		void Dispose()
		{
			{
				auto bookkeeping = _bookkeeping.get_access();
				bookkeeping->keyToRecord.clear();
				bookkeeping->masterSet.clear();
				bookkeeping->releasedKeys.clear();
				bookkeeping->nextHandle = UidHandle{};
			}
			_pool.clear();
		}

		mem::sptr<UidHandle> CreateUid()
		{
			mem::sptr<UidHandle> hndl = nullptr;
			mem::sptr<Uid> id = _pool.create_object();

			if (id)
			{
				auto bookkeeping = _bookkeeping.get_access();
				do
					*id = bookkeeping->generator();
				while (bookkeeping->masterSet.count(*id));
				bookkeeping->masterSet.emplace(*id);

				const UidHandle next{GetNextKey(*bookkeeping), GetNextGeneration(*bookkeeping)};
				HndlBlock* blocks = mem::create<HndlBlock>(_allocator);
				UidHandle* object = mem::construct<UidHandle>(blocks->object_block, next);
				hndl = mem::sptr<UidHandle>(mem::construct<HndlResource>(blocks->resource_block, _destroyer, object));
				bookkeeping->keyToRecord.emplace(hndl->key, UidRecord{id, hndl->generation});
			}

			return hndl;
		}

		Uid GetUid(mem::sptr<UidHandle> hndl)
		{
			mem::sptr<Uid> id = nullptr;
			if (hndl && hndl->IsValid())
			{
				auto bookkeeping = _bookkeeping.get_access();
				auto it = bookkeeping->keyToRecord.find(hndl->key);
				if (it != bookkeeping->keyToRecord.end() && hndl->generation == it->second.generation)
					id = it->second.uidSptr;
			}
			return id ? *id : Uid{};
		}

		bl Has(uid::Uid id)
		{
			auto bookkeeping = _bookkeeping.get_access();
			return bookkeeping->masterSet.find(id) != bookkeeping->masterSet.end();
		}
	};
} // namespace np::uid

#endif /* NP_ENGINE_UID_HPP */