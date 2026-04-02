//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/15/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_POOL_ALLOCATOR_HPP
#define NP_ENGINE_MEM_POOL_ALLOCATOR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "BlockedAllocator.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	template <typename T, siz ALIGNMENT>
	class pool_allocator_interface : public blocked_allocator
	{
	public:
		constexpr static siz CHUNK_ALIGNMENT = sanitize_alignment(ALIGNMENT);
		constexpr static siz CHUNK_SIZE = calc_aligned_size(sizeof(T), CHUNK_ALIGNMENT);

	protected:
		virtual void* get_chunk_ptr(void* ptr)
		{
			void* chunk = nullptr;
			if (contains(ptr))
			{
				//get the previous chunk ptr whose offset from _block.begin is a multiple of CHUNK_SIZE
				siz chunk_offset_count = (static_cast<ui8*>(ptr) - static_cast<ui8*>(_block.begin())) / CHUNK_SIZE; //truncate on purpose
				chunk = static_cast<ui8*>(_block.begin()) + chunk_offset_count * CHUNK_SIZE;
			}
			return chunk;
		}

	public:
		pool_allocator_interface(block b) : blocked_allocator(b)
		{}

		pool_allocator_interface(siz size, siz alignment) : blocked_allocator(size, alignment)
		{}

		virtual ~pool_allocator_interface() = default;

		virtual siz get_chunk_count() const
		{
			return _block.size / get_chunk_size();
		}

		virtual siz get_chunk_size() const
		{
			return CHUNK_SIZE;
		}
	};

	template <typename T, siz ALIGNMENT>
	class pool_allocator : public pool_allocator_interface<T, ALIGNMENT>
	{
	protected:
		using base = pool_allocator_interface<T, ALIGNMENT>;

		atm<void*> _allocation;

		virtual bl is_chunk_ptr(void* ptr)
		{
			return base::contains(ptr) && ((ui8*)ptr - (ui8*)base::_block.begin()) % base::get_chunk_size() == 0;
		}

		virtual void init()
		{
			block b{nullptr, base::get_chunk_size() };
			for (siz i = 0; i < base::get_chunk_count() - 1; i++)
			{
				b.ptr = &static_cast<ui8*>(base::_block.ptr)[i * base::get_chunk_size()];
				mem::construct<void*>(b, &static_cast<ui8*>(base::_block.ptr)[(i + 1) * base::get_chunk_size()]);
			}

			b.ptr = &static_cast<ui8*>(base::_block.ptr)[(base::get_chunk_count() - 1) * base::get_chunk_size()];
			mem::construct<void*>(b, nullptr);
			_allocation.store(base::_block.ptr, mo_release);
		}

	public:
		pool_allocator(block b): base(b)
		{
			init();
		}

		pool_allocator(siz size, siz alignment): base(size, alignment)
		{
			init();
		}

		virtual ~pool_allocator() = default;

		virtual block allocate(siz size, siz alignment) override
		{
			block b{};
			if (calc_aligned_size(size, alignment) <= base::get_chunk_size())
			{
				void* allocated = _allocation.load(mo_acquire);
				while (allocated &&
					   !_allocation.compare_exchange_weak(allocated, *(void**)(&static_cast<ui8*>(allocated)[0]), mo_release,
														  mo_relaxed))
				{}

				if (allocated)
					b = { allocated, base::get_chunk_size() };
			}
			return b;
		}

		virtual block reallocate(block& b_, siz size, siz alignment) override
		{
			block b = allocate(size, alignment);
			if (base::contains(b_))
			{
				const siz byte_count = b.size < b_.size ? b.size : b_.size;
				copy_bytes(b.begin(), b_.begin(), byte_count);
				deallocate(b_);
				b_.invalidate();
			}
			return b;
		}

		/*
			this value is meaningless from pool_allocator
		*/
		virtual block reallocate(void* ptr, siz size, siz alignment) override
		{
			return {};
		}

		virtual bl deallocate(block& b) override
		{
			bl deallocated = false;
			if (is_chunk_ptr(b.ptr) && b.size == base::get_chunk_size())
			{
				mem::construct<void*>(b, _allocation.load(mo_acquire));

				while (!_allocation.compare_exchange_weak(*(void**)(&static_cast<ui8*>(b.ptr)[0]), b.ptr, mo_release,
														  mo_relaxed))
				{}

				b.invalidate();
				deallocated = true;
			}
			return deallocated;
		}

		virtual bl deallocate(void* ptr) override
		{
			block b{ ptr, base::get_chunk_size() };
			return deallocate(b);
		}

		virtual bl deallocate_all() override
		{
			_allocation.store(nullptr, mo_release);
			init();
			return true;
		}
	};

	template <typename T, siz ALIGNMENT>
	class locking_pool_allocator : public pool_allocator_interface<T, ALIGNMENT>
	{
	protected:
		using base = pool_allocator_interface<T, ALIGNMENT>;

		mutex _mutex;
		void* _alloc_iterator;
		bl _deallocation_true_sort_false_constant;

		virtual bl is_chunk_ptr(void* ptr)
		{
			return base::contains(ptr) && ((ui8*)ptr - (ui8*)base::_block.begin()) % base::get_chunk_size() == 0;
		}

		virtual void init()
		{
			block b{ nullptr, base::get_chunk_size() };
			for (siz i = 0; i < base::get_chunk_count() - 1; i++)
			{
				b.ptr = &static_cast<ui8*>(base::_block.ptr)[i * base::get_chunk_size()];
				mem::construct<void*>(b, &static_cast<ui8*>(base::_block.ptr)[(i + 1) * base::get_chunk_size()]);
			}

			b.ptr = &static_cast<ui8*>(base::_block.ptr)[(base::get_chunk_count() - 1) * base::get_chunk_size()];
			mem::construct<void*>(b, nullptr);
			_alloc_iterator = base::_block.ptr;
		}

	public:
		locking_pool_allocator(block b) : base(b), _deallocation_true_sort_false_constant(false)
		{
			init();
		}

		locking_pool_allocator(siz size, siz alignment) : base(size, alignment), _deallocation_true_sort_false_constant(false)
		{
			init();
		}

		virtual ~locking_pool_allocator() = default;

		virtual bl get_deallocation_reclaim_behavior() const
		{
			return _deallocation_true_sort_false_constant;
		}

		virtual void set_deallocation_reclaim_behavior(bl true_sort_false_constant = false)
		{
			_deallocation_true_sort_false_constant = true_sort_false_constant;
		}

		virtual block allocate(siz size, siz alignment) override
		{
			scoped_lock l(_mutex);
			block b{};

			if (calc_aligned_size(size, alignment) <= base::get_chunk_size() && _alloc_iterator != nullptr)
			{
				b = { _alloc_iterator, base::get_chunk_size() };
				_alloc_iterator = *(void**)(&static_cast<ui8*>(_alloc_iterator)[0]);
			}

			return b;
		}

		virtual block reallocate(block& b_, siz size, siz alignment) override
		{
			block b = allocate(size, alignment);
			if (base::contains(b_))
			{
				const siz byte_count = b.size < b_.size ? b.size : b_.size;
				copy_bytes(b.begin(), b_.begin(), byte_count);
				deallocate(b_);
				b_.invalidate();
			}
			return b;
		}

		/*
			this value is meaningless from locking_pool_allocator
		*/
		virtual block reallocate(void* ptr, siz size, siz alignment) override
		{
			return {};
		}

		virtual bl deallocate(block& b, bl true_sort_false_constant)
		{
			scoped_lock l(_mutex);
			bl deallocated = false;

			if (is_chunk_ptr(b.ptr) && b.size == base::get_chunk_size())
			{
				void** deallocation_address = nullptr;

				if (true_sort_false_constant && _alloc_iterator != nullptr)
				{
					for (void** it = (void**)_alloc_iterator; base::contains(it); it = (void**)*it)
					{
						deallocation_address = it;
						if (*it > b.ptr)
							break;
					}

					NP_ENGINE_ASSERT(deallocation_address != nullptr, "our sorted insert failed");
				}
				else
				{
					deallocation_address = &_alloc_iterator;
				}

				mem::construct<void*>(b, *deallocation_address);
				*deallocation_address = b.ptr;
				b.invalidate();
				deallocated = true;
			}

			return deallocated;
		}

		virtual bl deallocate(block& b) override
		{
			return deallocate(b, _deallocation_true_sort_false_constant);
		}

		virtual bl deallocate(void* ptr, bl true_sort_false_constant)
		{
			block b{ ptr, base::get_chunk_size() };
			return deallocate(b, true_sort_false_constant);
		}

		virtual bl deallocate(void* ptr) override
		{
			block b{ ptr, base::get_chunk_size() };
			return deallocate(b);
		}

		virtual bl deallocate_all() override
		{
			scoped_lock l(_mutex);
			init();
			return true;
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_MEM_POOL_ALLOCATOR_HPP */
