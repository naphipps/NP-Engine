//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/1/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_UID_SYSTEM_HPP
#define NP_ENGINE_UID_SYSTEM_HPP

#ifndef NP_ENGINE_UID_SYSTEM_POOL_DEFAULT_SIZE
	#define NP_ENGINE_UID_SYSTEM_POOL_DEFAULT_SIZE 500 //TODO: use the default size from AccumulatingPool
#endif

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "UidImpl.hpp"
#include "UidPool.hpp"
#include "UidGenerator.hpp"
#include "UidHandle.hpp"

namespace np::uid
{
	class UidSystem
	{
	private:
		struct UidRecord
		{
			Uid* UidPtr = nullptr;
			UidHandle::GenerationType Generation = NP_ENGINE_UID_HANDLE_INVALID_GENERATION;

			bl IsValid() const
			{
				return UidPtr != nullptr && Generation != NP_ENGINE_UID_HANDLE_INVALID_GENERATION;
			}

			operator bl() const
			{
				return IsValid();
			}

			void Invalidate()
			{
				UidPtr = nullptr;
				Generation = NP_ENGINE_UID_HANDLE_INVALID_GENERATION;
			}
		};

		constexpr static siz UID_SIZE = mem::CalcAlignedSize(sizeof(Uid));

		Mutex _m; // TODO: don't know if an atomic lock would be faster??
		mem::TraitAllocator _allocator;
		UidGenerator _uid_gen;
		con::vector<mem::Block> _uid_pool_blocks;
		con::vector<UidPool*> _uid_pools;
		UidHandle::KeyType _key;
		UidHandle::GenerationType _generation;
		con::uset<UidHandle::KeyType> _released_keys;
		con::umap<UidHandle::KeyType, UidRecord> _key_to_record;
		con::uset<Uid> _uid_master_set;

		void AddPool()
		{
			_uid_pool_blocks.emplace_back(_allocator.Allocate(UID_SIZE * NP_ENGINE_UID_SYSTEM_POOL_DEFAULT_SIZE));
			_uid_pools.emplace_back(mem::Create<UidPool>(_allocator, _uid_pool_blocks.back()));
		}

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
				do ++_key;
				while (_key == NP_ENGINE_UID_HANDLE_INVALID_KEY);
				k = _key;
			}

			return k;
		}

		UidHandle::GenerationType GetNextGeneration()
		{
			do ++_generation;
			while (_generation == NP_ENGINE_UID_HANDLE_INVALID_GENERATION);
			return _generation;
		}

		void AttemptToCreateHandle(UidPool& pool, UidHandle& h, UidRecord& r)
		{
			r.UidPtr = pool.CreateObject();
			if (r.UidPtr)
			{
				do *r.UidPtr = _uid_gen();
				while (_uid_master_set.count(*r.UidPtr));
				_uid_master_set.emplace(*r.UidPtr);

				h = { GetNextKey(), GetNextGeneration() };
				r.Generation = h.Generation;
				_key_to_record.emplace(h.Key, r);
			}
		}

	public:
		UidSystem(): _key(NP_ENGINE_UID_HANDLE_INVALID_KEY), _generation(NP_ENGINE_UID_HANDLE_INVALID_GENERATION)
		{
			AddPool();
		}

		~UidSystem()
		{
			Dispose();
		}

		void Dispose()
		{
			Lock lock(_m);

			_key_to_record.clear();

			for (mem::Block& block : _uid_pool_blocks)
				_allocator.Deallocate(block);

			_uid_pool_blocks.clear();
			_uid_pools.clear();
		}

		UidHandle CreateUidHandle()
		{
			Lock lock(_m);

			UidHandle h{};
			UidRecord r{};

			for (auto it = _uid_pools.begin(); it != _uid_pools.end() && !h.IsValid(); it++)
				AttemptToCreateHandle(**it, h, r);

			if (!h.IsValid())
			{
				AddPool();
				AttemptToCreateHandle(*_uid_pools.back(), h, r);
			}

			return h;
		}

		Uid GetUid(const UidHandle& h)
		{
			Lock lock(_m);
			Uid* id = nullptr;

			if (h.IsValid())
			{
				auto it = _key_to_record.find(h.Key);
				if (it != _key_to_record.end() && h.Generation == it->second.Generation)
					id = it->second.UidPtr;
			}

			return id ? *id : Uid{};
		}

		void DestroyUidHandle(UidHandle& h)
		{
			Lock lock(_m);

			if (h.IsValid())
			{
				auto it = _key_to_record.find(h.Key);
				if (it != _key_to_record.end())
				{
					UidRecord r = it->second;
					if (h.Generation == r.Generation)
						for (UidPool*& pool : _uid_pools)
							if (pool->Contains(r.UidPtr))
							{
								_key_to_record.erase(it);
								pool->DestroyObject(r.UidPtr);
								_released_keys.emplace(h.Key);
								h.Invalidate();
								break;
							}
				}
			}
		}

		void Defragment()
		{
			Lock lock(_m);
			// TODO: defragment our pools by putting all our uid's to the front pools, then removing all empty pools
		}
	};
} // namespace np::uid

#endif /* NP_ENGINE_UID_SYSTEM_HPP */