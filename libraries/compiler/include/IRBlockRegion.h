
#pragma once

#include "IRInclude.h"
#include "IREmitter.h"

#include <memory>

namespace emll
{
	namespace compiler
	{
		class IRBlockRegion
		{
		public:
			IRBlockRegion(llvm::BasicBlock* pStart);

			llvm::BasicBlock* Start() const { return _pStart; }
			llvm::BasicBlock* End() const { return _pEnd; }
			void SetEnd(llvm::BasicBlock* pEnd);
			bool IsSingleBlock() const;

		private:
			llvm::BasicBlock* _pStart = nullptr;
			llvm::BasicBlock* _pEnd = nullptr;
		};

		class IRFunctionEmitter;

		class IRBlockRegionList
		{
		public:
			IRBlockRegionList(IRFunctionEmitter& fn);

			IRBlockRegion* Add(llvm::BasicBlock* pStart);

			void Link(IRBlockRegion* pTopRegion, IRBlockRegion* pBottomRegion);
			void Link();

		private:

		private:
			IRFunctionEmitter& _fn;
			std::vector<std::shared_ptr<IRBlockRegion>> _regions;
		};
	}
}