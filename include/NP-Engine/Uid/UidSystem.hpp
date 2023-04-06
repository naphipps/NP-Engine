//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/1/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_UID_SYSTEM_HPP
#define NP_ENGINE_UID_SYSTEM_HPP

#ifndef NP_ENGINE_UID_SYSTEM_POOL_DEFAULT_SIZE
	#define NP_ENGINE_UID_SYSTEM_POOL_DEFAULT_SIZE 500 // TODO: use the default size from AccumulatingPool
#endif

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "UidImpl.hpp"
#include "UidGenerator.hpp"
#include "UidHandle.hpp"

namespace np::uid
{
	class UidSystem
	{
	public:
		
		class SmartPtrDestroyer : public mem::smart_contiguous_destroyer<UidHandle>
		{
		public:
			using base = mem::smart_contiguous_destroyer<UidHandle>;

		private:
			UidSystem& _uid_system;

		public:
			SmartPtrDestroyer(UidSystem& uid_system, mem::Allocator& allocator) : base(allocator), _uid_system(uid_system) {}

			SmartPtrDestroyer(const SmartPtrDestroyer& other) : base(other), _uid_system(other._uid_system) {}

			SmartPtrDestroyer(SmartPtrDestroyer&& other) noexcept : base(other), _uid_system(other._uid_system) {}

			void destruct_object(UidHandle* ptr) override
			{
				_uid_system.ReleaseHandle(ptr);
				base::destruct_object(ptr);
			}
		};

		using SmartPtrResource = mem::smart_ptr_resource<UidHandle, SmartPtrDestroyer>;
		using SmartPtrContiguousBlock = mem::smart_ptr_contiguous_block<UidHandle, SmartPtrResource>;

	private:
		struct UidRecord
		{
			mem::sptr<Uid> UidPtr = nullptr; //TODO: camelCase these fields
			UidHandle::GenerationType Generation = NP_ENGINE_UID_HANDLE_INVALID_GENERATION;

			bl IsValid() const
			{
				return UidPtr && Generation != NP_ENGINE_UID_HANDLE_INVALID_GENERATION;
			}

			operator bl() const
			{
				return IsValid();
			}

			void Invalidate()
			{
				UidPtr.reset();
				Generation = NP_ENGINE_UID_HANDLE_INVALID_GENERATION;
			}
		};

		constexpr static siz UID_SIZE = mem::CalcAlignedSize(sizeof(Uid));

		Mutex _m;
		mem::TraitAllocator _allocator;
		SmartPtrDestroyer _uid_handle_destroyer;
		UidGenerator _uid_gen;
		UidPool _uid_pool;
		UidHandle _next_handle;
		con::uset<UidHandle::KeyType> _released_keys;
		con::umap<UidHandle::KeyType, UidRecord> _key_to_record;
		con::uset<Uid> _uid_master_set;

		UidHandle::KeyType GetNextKey()
		{
			UidHandle::KeyType k = NP_ENGINE_UID_HANDLE_INVALID_KEY;

			if (_released_keys.size())
			{
				k = *_released_keys.begin();
				_released_keys.erase(_released_keys.begin());
			}
			else
			{
				do
					++_next_handle.Key;
				while (_next_handle.Key == NP_ENGINE_UID_HANDLE_INVALID_KEY);
				k = _next_handle.Key;
			}

			return k;
		}

		UidHandle::GenerationType GetNextGeneration()
		{
			do
				++_next_handle.Generation;
			while (_next_handle.Generation == NP_ENGINE_UID_HANDLE_INVALID_GENERATION);
			return _next_handle.Generation;
		}

		void ReleaseHandle(UidHandle* hndl_ptr)
		{
			if (hndl_ptr)
			{
				Lock lock(_m);

				UidHandle& hndl = *hndl_ptr;
				if (hndl.IsValid())
				{
					auto it = _key_to_record.find(hndl.Key);
					if (it != _key_to_record.end())
					{
						UidRecord record = it->second;
						if (hndl.Generation == record.Generation && _uid_pool.Contains(record.UidPtr))
						{
							record.Invalidate();
							_key_to_record.erase(it);
							_released_keys.emplace(hndl.Key);
							hndl.Invalidate();
						}
					}
				}
			}
		}

	public:
		UidSystem() : _uid_handle_destroyer(*this, _allocator) {}

		~UidSystem()
		{
			Dispose();
		}

		void Dispose()
		{
			Lock lock(_m);
			_uid_pool.Clear();
			_next_handle = UidHandle{};
			_released_keys.clear();
			_key_to_record.clear();
			_uid_master_set.clear();
		}

		mem::sptr<UidHandle> CreateUidHandle()
		{
			Lock lock(_m);
			mem::sptr<UidHandle> hndl = nullptr;
			mem::sptr<Uid> id = _uid_pool.CreateObject();

			if (id)
			{
				do
					*id = _uid_gen();
				while (_uid_master_set.count(*id));
				_uid_master_set.emplace(*id);

				SmartPtrContiguousBlock* contiguous_block = mem::Create<SmartPtrContiguousBlock>(_allocator);
				UidHandle* object = mem::Construct<UidHandle>(contiguous_block->object_block, UidHandle{ GetNextKey(), GetNextGeneration() });
				hndl = mem::sptr<UidHandle>(mem::Construct<SmartPtrResource>(contiguous_block->resource_block, _uid_handle_destroyer, object));

				_key_to_record.emplace(hndl->Key, UidRecord{ id, hndl->Generation });
			}

			return hndl;
		}

		Uid GetUid(mem::sptr<UidHandle> hndl)
		{
			Lock lock(_m);
			mem::sptr<Uid> id = nullptr;

			if (hndl->IsValid())
			{
				auto it = _key_to_record.find(hndl->Key);
				if (it != _key_to_record.end() && hndl->Generation == it->second.Generation)
					id = it->second.UidPtr;
			}

			return id ? *id : Uid{};
		}
	};
} // namespace np::uid

#endif /* NP_ENGINE_UID_SYSTEM_HPP */