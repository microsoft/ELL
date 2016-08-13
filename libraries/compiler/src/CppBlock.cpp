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

		void CppBlockList::Push(CppBlock* pBlock)
		{
			assert(pBlock != nullptr);
			_blocks.push_back(pBlock);
		}
		
		void CppBlockList::Remove(CppBlock* pBlock)
		{
			assert(pBlock != nullptr);
			_blocks.remove(pBlock);
		}

		void CppBlockList::Clear()
		{
			_blocks.clear();
		}

		void CppBlockList::Merge()
		{
			CppBlock* pFirst = First();
			for (auto pBlock = _blocks.begin(); pBlock != _blocks.end(); ++pBlock)
			{
				if (*pBlock != pFirst)
				{
					pFirst->Append(*pBlock);
				}
			}
			_blocks.clear();
			_blocks.push_back(pFirst);
		}
	}
}