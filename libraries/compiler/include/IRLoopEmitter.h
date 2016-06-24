#pragma once

#include "IREmitter.h"

namespace emll
{
	namespace compiler
	{
		class IRFunctionEmitter;

		class IRForLoopEmitter
		{
		public:
			IRForLoopEmitter(IRFunctionEmitter& fn)
				: _fn(fn) {}

			llvm::BasicBlock* Body()
			{
				return _pBodyBlock;
			}
			llvm::Value* IterationVar()
			{
				return _pIterationVar;
			}

			llvm::BasicBlock* Begin(const int repeatCount)
			{
				return Begin(0, repeatCount, 1);
			}
			llvm::BasicBlock* Begin(const int iStartAt, const int iMaxValue, const int stepSize);
			void End();

		private:
			void CreateBlocks();
			void EmitIterationVar(ValueType type, llvm::Value* pStartValue);
			void EmitCondition(ComparisonType type, llvm::Value* pTestValue);
			void EmitIncrement(ValueType type, llvm::Value* pIncValue);
			llvm::BasicBlock* PrepareBody();

		private:
			IRFunctionEmitter& _fn;
			llvm::BasicBlock* _pInitBlock = nullptr;
			llvm::BasicBlock* _pConditionBlock = nullptr;
			llvm::BasicBlock* _pBodyBlock = nullptr;
			llvm::BasicBlock* _pIncBlock = nullptr;
			llvm::BasicBlock* _pAfterBlock = nullptr;
			llvm::Value* _pIterationVar = nullptr;
		};
	}
}
