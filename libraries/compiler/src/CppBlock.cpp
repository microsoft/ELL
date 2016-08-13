#include "CppBlock.h"
#include <cassert>

namespace emll
{
	namespace compiler
	{
		CppBlock::CppBlock(int blockId)
			: _id(blockId)
		{
		}

		std::string CppBlock::IdString()
		{
			return "Block" + std::to_string(_id);
		}

		CppBlock* CppBlockAllocator::Alloc()
		{
			auto block = std::make_shared<CppBlock>(NextId());
			_allocatedBlocks[block->Id()] = block;	
			return block.get();
		}

		void CppBlockAllocator::Free(CppBlock* pBlock)
		{
			assert(pBlock != nullptr);

			_allocatedBlocks.erase(pBlock->Id());
		}

		void CppBlockAllocator::Clear()
		{
			_allocatedBlocks.clear();
			_nextId = 0;
		}

		CppBlock* CppBlockList::First()
		{
			return _blocks.front();
		}

		CppBlock* CppBlockList::Last()
		{
			return _blocks.back();
		}

		CppBlock* CppBlockList::AddBlock()
		{
			CppBlock* pBlock = _allocator.Alloc();
			_blocks.push_back(pBlock);
			return pBlock;
		}
		
		void CppBlockList::Remove(CppBlock* pBlock)
		{
			assert(pBlock != nullptr);
			_blocks.remove(pBlock);
			_allocator.Free(pBlock);
		}

		void CppBlockList::Clear()
		{
			_blocks.clear();
			_allocator.Clear();
		}

		CppBlock* CppBlockList::Merge()
		{
			CppBlock* pMergedBlock = _allocator.Alloc();
			for (auto pBlock = _blocks.begin(); pBlock != _blocks.end(); ++pBlock)
			{
				pMergedBlock->Append(*pBlock);
				_allocator.Free(*pBlock);
			}
			_blocks.clear();
			_blocks.push_back(pMergedBlock);
			return pMergedBlock;
		}
	}
}