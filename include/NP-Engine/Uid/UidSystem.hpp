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
		constexpr static siz UID_ALIGNED_SIZE = mem::CalcAlignedSize(sizeof(Uid));

		Mutex _m; //TODO: don't know if an atomic lock would be faster??
		UidGenerator _uid_gen;
		mem::TraitAllocator _allocator;
		con::vector<mem::Block> _uid_pool_blocks;
		con::vector<UidPool*> _uid_pools;
		con::umap<void*, UidHandle> _key_to_record;
		siz _generation;

		void AddPool()
		{
			_uid_pool_blocks.emplace_back(_allocator.Allocate(UID_ALIGNED_SIZE * NP_ENGINE_UID_SYSTEM_POOL_DEFAULT_SIZE));
			_uid_pools.emplace_back(mem::Create<UidPool>(_allocator, _uid_pool_blocks.back()));
		}

	public:

		UidSystem() : _generation(1)
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
			Uid* uid = nullptr;

			for (UidPool*& pool : _uid_pools)
			{
				uid = pool->CreateObject();
				if (uid)
				{
					*uid = _uid_gen();
					h = { uid, _generation++ };
					_key_to_record.emplace(uid, h);
					break;
				}
			}

			if (!uid)
			{
				AddPool();
				uid = _uid_pools.back()->CreateObject();
				if (uid)
				{
					*uid = _uid_gen();
					h = { uid, _generation++ };
					_key_to_record.emplace(uid, h);
				}
			}

			if (_generation == 0)
				_generation++;

			return h;
		}

		Uid GetUid(const UidHandle& h)
		{
			Lock lock(_m);
			Uid* uid = nullptr;

			auto it = _key_to_record.find(h.Key);
			if (it != _key_to_record.end() && h.Generation == it->second.Generation)
				uid = (Uid*)it->second.Key;

			return uid ? *uid : Uid{};
		}

		void DestroyUidHandle(UidHandle& h)
		{
			Lock lock(_m);

			auto it = _key_to_record.find(h.Key);
			if (it != _key_to_record.end() && h.Generation == it->second.Generation)
			{
				Uid* uid = (Uid*)it->second.Key;
				_key_to_record.erase(uid);
				for (UidPool*& pool : _uid_pools)
					if (pool->Contains(uid))
					{
						pool->DestroyObject(uid);
						break;
					}

				h.Key = nullptr;
			}
		}
	};
}

#endif /* NP_ENGINE_UID_SYSTEM_HPP */