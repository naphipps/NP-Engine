//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/25/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_EXPLICIT_LIST_ALLOCATOR_HPP
#define NP_ENGINE_EXPLICIT_LIST_ALLOCATOR_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "BookkeepingAllocator.hpp"
#include "Margin.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	namespace __detail
	{
		struct ExplicitListAllocatorNode
		{
			ExplicitListAllocatorNode* Next = nullptr;
			ExplicitListAllocatorNode* Prev = nullptr;
		};
	} // namespace __detail

	class ExplicitListAllocator : public BookkeepingAllocator
	{
	private:
		using Node = __detail::ExplicitListAllocatorNode;
		using NodePtr = Node*;
		using Margin = __detail::Margin;
		using MarginPtr = __detail::MarginPtr;

		constexpr static siz MARGIN_SIZE = __detail::MARGIN_SIZE;
		constexpr static siz NODE_SIZE = CalcAlignedSize(sizeof(Node));

	public:
		constexpr static siz BOOKKEEPING_SIZE = (MARGIN_SIZE * 2);
		constexpr static siz OVERHEAD_SIZE = BOOKKEEPING_SIZE + NODE_SIZE;

	private:
		Mutex _mutex;
		NodePtr _root;

		void DetachNode(NodePtr node)
		{
			if (node->Next)
				node->Next->Prev = node->Prev;

			if (node->Prev)
				node->Prev->Next = node->Next;
			else
				_root = node->Next;
		}

		bl InitFreeBlock(Block& block)
		{
			bl initialized = false;
			if (block.size >= OVERHEAD_SIZE)
			{
				Block header_block{block.Begin(), MARGIN_SIZE};
				Block footer_block{(ui8*)block.End() - MARGIN_SIZE, MARGIN_SIZE};
				Construct<Margin>(header_block);
				Construct<Margin>(footer_block);
				MarginPtr header = static_cast<MarginPtr>(header_block.ptr);
				MarginPtr footer = static_cast<MarginPtr>(footer_block.ptr);
				header->Size = block.size;
				*footer = *header;

				Block node_block{header_block.End(), NODE_SIZE};
				Construct<Node>(node_block);

				initialized = true;
			}

			return initialized;
		}

		void StowFreeBlock(Block& block)
		{
			bl init_success = InitFreeBlock(block);
			NP_ENGINE_ASSERT(init_success, "our init here should always succeed");
			NodePtr node = (NodePtr)((ui8*)block.Begin() + MARGIN_SIZE);

			NodePtr insert = _root;
			while (Contains(insert) && insert->Next && insert->Next < node->Next)
				insert = insert->Next;

			node->Next = insert ? insert->Next : nullptr;
			node->Prev = insert;

			if (node->Next)
				node->Next->Prev = node;

			if (node->Prev)
				node->Prev->Next = node;
			else
				_root = node;
		}

		MarginPtr FindAllocationHeader(siz size, bl true_best_false_first)
		{
			MarginPtr node_header, header = nullptr;
			NodePtr node;

			if (true_best_false_first)
			{
				siz diff, min_diff = SIZ_MAX;
				for (node = _root; Contains(node); node = node->Next)
				{
					node_header = (MarginPtr)((ui8*)node - MARGIN_SIZE);
					if (!node_header->IsAllocated && node_header->Size >= size)
					{
						diff = node_header->Size - size;
						if (diff == 0)
						{
							header = node_header;
							break;
						}
						else if (diff < min_diff)
						{
							header = node_header;
							min_diff = diff;
						}
					}
				}
			}
			else
			{
				for (node = _root; Contains(node) && !header; node = node->Next)
				{
					node_header = (MarginPtr)((ui8*)node - MARGIN_SIZE);
					if (!node_header->IsAllocated && node_header->Size >= size)
						header = node_header;
				}
			}

			return header;
		}

		Block InternalAllocate(siz size, bl true_best_false_first)
		{
			Block block{}, split{};
			size = CalcAlignedSize(size) + BOOKKEEPING_SIZE;
			if (size < OVERHEAD_SIZE)
				size = OVERHEAD_SIZE;

			MarginPtr header = FindAllocationHeader(size, true_best_false_first);
			if (header)
			{
				siz size_check = header->Size;
				block = {header, header->Size};
				DetachNode((NodePtr)((ui8*)header + MARGIN_SIZE));

				// can we split?
				if (block.size - size >= OVERHEAD_SIZE)
				{
					split = {(ui8*)block.Begin() + size, block.size - size};
					block.size -= split.size;

					bl success = InitFreeBlock(split);
					NP_ENGINE_ASSERT(success, "InitFreeBlock must be successful here");

					success = InitFreeBlock(block);
					NP_ENGINE_ASSERT(success, "InitFreeBlock must be successful here");
				}

				header->IsAllocated = true;
				MarginPtr footer = (MarginPtr)((ui8*)block.End() - MARGIN_SIZE);
				*footer = *header;

				// only deallocate split after our block is considered allocated
				if (split.IsValid())
				{
					bl success = InternalDeallocate((ui8*)split.Begin() + MARGIN_SIZE);
					NP_ENGINE_ASSERT(success, "InternalDeallocate here must succeed");
				}

				block.ptr = (ui8*)block.Begin() + MARGIN_SIZE;
				block.size -= BOOKKEEPING_SIZE;
			}

			return block;
		}

		bl InternalDeallocate(void* ptr)
		{
			bl deallocated = false;
			if (Contains(ptr))
			{
				MarginPtr header = (MarginPtr)((ui8*)ptr - MARGIN_SIZE);
				header->IsAllocated = false;

				// claim previous blocks
				for (MarginPtr prev_footer = (MarginPtr)((ui8*)header - MARGIN_SIZE); Contains(prev_footer);
					 prev_footer = (MarginPtr)((ui8*)header - MARGIN_SIZE))
				{
					if (!prev_footer->IsAllocated)
					{
						MarginPtr claim_header = (MarginPtr)((ui8*)prev_footer + MARGIN_SIZE - prev_footer->Size);
						claim_header->Size = claim_header->Size + header->Size;
						MarginPtr claim_footer = (MarginPtr)((ui8*)claim_header + claim_header->Size - MARGIN_SIZE);
						*claim_footer = *claim_header;
						header = claim_header;

						NodePtr claim_node = (NodePtr)((ui8*)claim_header + MARGIN_SIZE);
						DetachNode(claim_node);
						continue;
					}
					break;
				}

				// claim next blocks
				for (MarginPtr next_header = (MarginPtr)((ui8*)header + header->Size); Contains(next_header);
					 next_header = (MarginPtr)((ui8*)header + header->Size))
				{
					if (!next_header->IsAllocated)
					{
						MarginPtr claim_header = next_header;
						header->Size = header->Size + claim_header->Size;
						MarginPtr claim_footer = (MarginPtr)((ui8*)header + header->Size - MARGIN_SIZE);
						*claim_footer = *header;

						NodePtr claim_node = (NodePtr)((ui8*)claim_header + MARGIN_SIZE);
						DetachNode(claim_node);
						continue;
					}
					break;
				}

				Block block{header, header->Size};
				StowFreeBlock(block);
				deallocated = true;
			}

			return deallocated;
		}

		void Init()
		{
			if (InitFreeBlock(_block))
			{
				_root = (NodePtr)((ui8*)_block.Begin() + MARGIN_SIZE);
			}
			else
			{
				_block.Zeroize();
				_root = nullptr;
			}
		}

	public:
		ExplicitListAllocator(Block block): BookkeepingAllocator(block), _root(nullptr)
		{
			Init();
		}

		ExplicitListAllocator(siz size): BookkeepingAllocator(size), _root(nullptr)
		{
			Init();
		}

		Block Allocate(siz size) override
		{
			return AllocateBest(size);
		}

		Block AllocateBest(siz size)
		{
			Lock lock(_mutex);
			return InternalAllocate(size, true);
		}

		Block AllocateFirst(siz size)
		{
			Lock lock(_mutex);
			return InternalAllocate(size, false);
		}

		Block ExtractBlock(void* block_ptr) const override
		{
			Block block;
			if (Contains(block_ptr))
			{
				MarginPtr block_header = (MarginPtr)((ui8*)block_ptr - MARGIN_SIZE);
				block = {block_ptr, block_header->Size - BOOKKEEPING_SIZE};
			}

			return block;
		}

		Block Reallocate(Block& old_block, siz new_size) override
		{
			return ReallocateBest(old_block, new_size);
		}

		Block Reallocate(void* old_ptr, siz new_size) override
		{
			return ReallocateBest(old_ptr, new_size);
		}

		Block ReallocateBest(Block& old_block, siz new_size)
		{
			Block new_block = AllocateBest(new_size);
			if (Contains(old_block))
			{
				CopyBytes(new_block.Begin(), old_block.Begin(), old_block.size);
				Deallocate(old_block);
				old_block.Invalidate();
			}

			return new_block;
		}

		Block ReallocateBest(void* old_ptr, siz new_size)
		{
			Block old_block = ExtractBlock(old_ptr);
			return ReallocateBest(old_block, new_size);
		}

		Block ReallocateFirst(Block& old_block, siz new_size)
		{
			Block new_block = AllocateFirst(new_size);
			if (Contains(old_block))
			{
				CopyBytes(new_block.Begin(), old_block.Begin(), old_block.size);
				Deallocate(old_block);
				old_block.Invalidate();
			}

			return new_block;
		}

		Block ReallocateFirst(void* old_ptr, siz new_size)
		{
			Block old_block = ExtractBlock(old_ptr);
			return ReallocateFirst(old_block, new_size);
		}

		bl Deallocate(Block& block) override
		{
			bl deallocated = Deallocate(block.ptr);
			if (deallocated)
				block.Invalidate();

			return deallocated;
		}

		bl Deallocate(void* ptr) override
		{
			Lock lock(_mutex);
			return InternalDeallocate(ptr);
		}

		bl DeallocateAll() override
		{
			Lock lock(_mutex);
			Init();
			return true;
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_EXPLICIT_LIST_ALLOCATOR_HPP */
