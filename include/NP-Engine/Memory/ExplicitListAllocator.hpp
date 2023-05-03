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
		using Margin = __detail::Margin;

		constexpr static siz MARGIN_SIZE = __detail::MARGIN_SIZE;
		constexpr static siz NODE_SIZE = CalcAlignedSize(sizeof(Node));

	public:
		constexpr static siz BOOKKEEPING_SIZE = (MARGIN_SIZE * 2);
		constexpr static siz OVERHEAD_SIZE = BOOKKEEPING_SIZE + NODE_SIZE;

	private:
		mutex _mutex;
		Node* _root;

		void DetachNode(Node* node)
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
				Margin* header = mem::Construct<Margin>(header_block);
				Margin* footer = mem::Construct<Margin>(footer_block);
				header->SetSize(block.size);
				*footer = *header;

				Block node_block{header_block.End(), NODE_SIZE};
				Node* node = mem::Construct<Node>(node_block);
				initialized = node;
			}

			return initialized;
		}

		void StowFreeBlock(Block& block)
		{
			bl init_success = InitFreeBlock(block);
			NP_ENGINE_ASSERT(init_success, "our init here should always succeed");
			Node* node = (Node*)((ui8*)block.Begin() + MARGIN_SIZE);

			Node* insert = _root;
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

		Margin* FindAllocationHeader(siz size, bl true_best_false_first)
		{
			Margin *node_header, *header = nullptr;
			Node* node;

			if (true_best_false_first)
			{
				siz diff, min_diff = SIZ_MAX;
				for (node = _root; Contains(node); node = node->Next)
				{
					node_header = (Margin*)((ui8*)node - MARGIN_SIZE);
					if (!node_header->IsAllocated() && node_header->GetSize() >= size)
					{
						diff = node_header->GetSize() - size;
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
					node_header = (Margin*)((ui8*)node - MARGIN_SIZE);
					if (!node_header->IsAllocated() && node_header->GetSize() >= size)
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

			Margin* header = FindAllocationHeader(size, true_best_false_first);
			if (header)
			{
				siz size_check = header->GetSize();
				block = {header, header->GetSize()};
				DetachNode((Node*)((ui8*)header + MARGIN_SIZE));

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

				header->SetIsAllocated(true);
				Margin* footer = (Margin*)((ui8*)block.End() - MARGIN_SIZE);
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
				Margin* header = (Margin*)((ui8*)ptr - MARGIN_SIZE);
				header->SetIsAllocated(false);

				Margin *prev_footer, *next_header, *claim_footer, *claim_header;
				Node* claim_node;

				// claim previous blocks
				for (prev_footer = (Margin*)((ui8*)header - MARGIN_SIZE); Contains(prev_footer);
					 prev_footer = (Margin*)((ui8*)header - MARGIN_SIZE))
				{
					if (!prev_footer->IsAllocated())
					{
						claim_header = (Margin*)((ui8*)prev_footer + MARGIN_SIZE - prev_footer->GetSize());
						claim_header->SetSize(claim_header->GetSize() + header->GetSize());
						claim_footer = (Margin*)((ui8*)claim_header + claim_header->GetSize() - MARGIN_SIZE);
						*claim_footer = *claim_header;
						header = claim_header;

						claim_node = (Node*)((ui8*)claim_header + MARGIN_SIZE);
						DetachNode(claim_node);
						continue;
					}
					break;
				}

				// claim next blocks
				for (next_header = (Margin*)((ui8*)header + header->GetSize()); Contains(next_header);
					 next_header = (Margin*)((ui8*)header + header->GetSize()))
				{
					if (!next_header->IsAllocated())
					{
						claim_header = next_header;
						header->SetSize(header->GetSize() + claim_header->GetSize());
						claim_footer = (Margin*)((ui8*)header + header->GetSize() - MARGIN_SIZE);
						*claim_footer = *header;

						claim_node = (Node*)((ui8*)claim_header + MARGIN_SIZE);
						DetachNode(claim_node);
						continue;
					}
					break;
				}

				Block block{header, header->GetSize()};
				StowFreeBlock(block);
				deallocated = true;
			}

			return deallocated;
		}

		void Init()
		{
			if (InitFreeBlock(_block))
			{
				_root = (Node*)((ui8*)_block.Begin() + MARGIN_SIZE);
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
			scoped_lock l(_mutex);
			return InternalAllocate(size, true);
		}

		Block AllocateFirst(siz size)
		{
			scoped_lock l(_mutex);
			return InternalAllocate(size, false);
		}

		Block ExtractBlock(void* block_ptr) override
		{
			Block block;
			if (Contains(block_ptr))
			{
				Margin* block_header = (Margin*)((ui8*)block_ptr - MARGIN_SIZE);
				block = {block_ptr, block_header->GetSize() - BOOKKEEPING_SIZE};
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
			scoped_lock l(_mutex);
			return InternalDeallocate(ptr);
		}

		bl DeallocateAll() override
		{
			scoped_lock l(_mutex);
			Init();
			return true;
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_EXPLICIT_LIST_ALLOCATOR_HPP */
