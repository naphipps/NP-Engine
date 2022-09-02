//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/1/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_UID_SYSTEM_HPP
#define NP_ENGINE_UID_SYSTEM_HPP

#ifndef NP_ENGINE_UID_SYSTEM_POOL_DEFAULT_SIZE
	#define NP_ENGINE_UID_SYSTEM_POOL_DEFAULT_SIZE 500
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
			UidHandle::GenerationType Generation = NP_ENGINE_UID_INVALID_GENERATION;

			bl IsValid() const
			{
				return UidPtr != nullptr && Generation != NP_ENGINE_UID_INVALID_GENERATION;
			}

			operator bl() const
			{
				return IsValid();
			}

			void Invalidate()
			{
				UidPtr = nullptr;
				Generation = NP_ENGINE_UID_INVALID_GENERATION;
			}
		};

		constexpr static siz UID_ALIGNED_SIZE = mem::CalcAlignedSize(sizeof(Uid));

		Mutex _m; //TODO: don't know if an atomic lock would be faster??
		UidGenerator _uid_gen;
		mem::TraitAllocator _allocator;
		con::vector<mem::Block> _uid_pool_blocks;
		con::vector<UidPool*> _uid_pools;
		con::umap<UidHandle::KeyType, UidRecord> _key_to_record;
		UidHandle::GenerationType _generation;
		con::uset<Uid> _uid_master_set;

		void AddPool()
		{
			_uid_pool_blocks.emplace_back(_allocator.Allocate(UID_ALIGNED_SIZE * NP_ENGINE_UID_SYSTEM_POOL_DEFAULT_SIZE));
			_uid_pools.emplace_back(mem::Create<UidPool>(_allocator, _uid_pool_blocks.back()));
		}

		void AttemptToCreateHandle(UidPool& pool, UidHandle& h, UidRecord& r)
		{
			r.UidPtr = pool.CreateObject();
			if (r.UidPtr)
			{
				*r.UidPtr = _uid_gen();
				while (_uid_master_set.count(*r.UidPtr))
					*r.UidPtr = _uid_gen();

				_uid_master_set.emplace(*r.UidPtr);
				h.Key = (siz)r.UidPtr; //TODO: we might would like to create a key in a better way? I don't think we need to
				h.Generation = ++_generation;
				r.Generation = h.Generation;
				_key_to_record.emplace(h.Key, r);
			}
		}

	public:

		UidSystem() : _generation(NP_ENGINE_UID_INVALID_GENERATION)
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
				AttemptToCreateHandle(*(*it), h, r);

			if (!h.IsValid())
			{
				AddPool();
				AttemptToCreateHandle(*_uid_pools.back(), h, r);
			}

			if (_generation == NP_ENGINE_UID_INVALID_GENERATION)
				_generation++;

			return h;
		}

		Uid GetUid(const UidHandle& h)
		{
			Lock lock(_m);
			Uid* uid = nullptr;

			if (h.IsValid())
			{
				auto it = _key_to_record.find(h.Key);
				if (it != _key_to_record.end() && h.Generation == it->second.Generation)
					uid = it->second.UidPtr;
			}

			return uid ? *uid : Uid{};
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
								h.Invalidate();
								break;
							}
				}
			}
		}

		void Defragment()
		{
			Lock lock(_m);

			//TODO: defragment our pools by putting all our uid's to the front pools, then removing all full pools

		}
	};
}

#endif /* NP_ENGINE_UID_SYSTEM_HPP */