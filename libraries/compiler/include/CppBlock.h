#pragma once

#include "CppEmitter.h"

#include <memory>
#include <vector>
#include <unordered_map>

namespace emll
{
	namespace compiler
	{
		///<summary> A block of Cpp code </summary>
		class CppBlock : public CppEmitter
		{
		public:
			CppBlock(int blockId);

			const int Id() const { return _id; }
			std::string IdString();

		private:
			int _id;
		};

		///<summary>Cpp code block allocation</summary>
		class CppBlockAllocator
		{
		public:
			CppBlock* Alloc();
			void Free(CppBlock* pBlock);
			void Clear();

		private:
			int NextId()
			{
				return ++_nextId;
			}
		private:
			std::unordered_map<int, std::shared_ptr<CppBlock>> _allocatedBlocks;
			int _nextId = 0;
		};

		///<sumary>List of Cpp code blocks. Used by functions </summary>
		class CppBlockList
		{
		public:
			CppBlock* AddBlock();

			CppBlock* First();
			CppBlock* Last();

			void Remove(CppBlock* pBlock);
			void Clear();
			CppBlock* Merge();

		private:
			std::list<CppBlock*> _blocks;
			CppBlockAllocator _allocator;
		};
	}
}
