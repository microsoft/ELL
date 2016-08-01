#include "IRLoopEmitter.h"
#include "IRFunctionEmitter.h"

namespace emll
{
	namespace compiler
	{
		const std::string ForInitBlockName = "for.init";
		const std::string ForConditionBlockName = "for.cond";
		const std::string ForBodyBlockName = "for.body";
		const std::string ForIncBlockName = "for.inc";
		const std::string ForAfterBlockName = "for.after";

		void IRForLoopEmitter::End()
		{
			assert(_pIncBlock != nullptr);

			// Caller is done generating the body. Add a branch from the Body block to the increment block
			_fn.Branch(_pIncBlock);
			_fn.CurrentBlock(_pAfterBlock);
		}

		void IRForLoopEmitter::Clear()
		{
			_pInitBlock = nullptr;
			_pConditionBlock = nullptr;
			_pBodyBlock = nullptr;
			_pIncBlock = nullptr;
			_pAfterBlock = nullptr;
			_pIterationVar = nullptr;
		}

		llvm::Value* IRForLoopEmitter::LoadIterationVar()
		{
			return _fn.Load(_pIterationVar);
		}

		llvm::BasicBlock* IRForLoopEmitter::Begin(const int iStartAt, const int iMaxValue, const int stepSize)
		{
			CreateBlocks();
			EmitIterationVar(ValueType::Int32, _fn.Literal(iStartAt));
			EmitCondition(ComparisonType::Lt, _fn.Literal(iMaxValue));
			EmitIncrement(ValueType::Int32, _fn.Literal(stepSize));
			return PrepareBody();
		}

		llvm::BasicBlock* IRForLoopEmitter::Begin(llvm::Value* pRepeatCount)
		{
			assert(pRepeatCount != nullptr);

			CreateBlocks();
			EmitIterationVar(ValueType::Int32, _fn.Literal(0));
			EmitCondition(ComparisonType::Lt, pRepeatCount);
			EmitIncrement(ValueType::Int32, _fn.Literal(1));
			return PrepareBody();
		}

		void IRForLoopEmitter::CreateBlocks()
		{
			_pInitBlock = _fn.Block(ForInitBlockName);
			_pConditionBlock = _fn.Block(ForConditionBlockName);
			_pBodyBlock = _fn.Block(ForBodyBlockName);
			_pIncBlock = _fn.Block(ForIncBlockName);
			_pAfterBlock = _fn.Block(ForAfterBlockName);
		}

		void IRForLoopEmitter::EmitIterationVar(ValueType type, llvm::Value* pStartValue)
		{
			_fn.Branch(_pInitBlock);
			_fn.CurrentBlock(_pInitBlock);
			_pIterationVar = _fn.Var(type);
			_fn.Store(_pIterationVar, pStartValue);
		}

		void IRForLoopEmitter::EmitCondition(ComparisonType type, llvm::Value* pTestValue)
		{
			_fn.Branch(_pConditionBlock);
			_fn.CurrentBlock(_pConditionBlock);
			_fn.Branch(type, _fn.Load(_pIterationVar), pTestValue, _pBodyBlock, _pAfterBlock);
		}

		void IRForLoopEmitter::EmitIncrement(ValueType type, llvm::Value* pIncValue)
		{
			_fn.CurrentBlock(_pIncBlock);
			_fn.OpAndUpdate(_pIterationVar, OperatorType::Add, pIncValue);
			_fn.Branch(_pConditionBlock);
		}

		llvm::BasicBlock* IRForLoopEmitter::PrepareBody()
		{
			_fn.CurrentBlock(_pBodyBlock);
			return _pBodyBlock;
		}
	}
}