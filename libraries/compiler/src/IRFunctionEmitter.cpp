////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IRFunctionEmitter.cpp (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRFunctionEmitter.h"
#include "IREmitter.h"
#include "CompilerException.h"

namespace emll
{
	namespace compiler
	{
		const std::string PrintfFnName = "printf";
		const std::string MallocFnName = "malloc";
		const std::string FreeFnName = "free";

		IRFunctionEmitter::IRFunctionEmitter(IREmitter* pEmitter, llvm::Function* pfn)
		{
			Init(pEmitter, pfn);
		}

		IRFunctionEmitter::IRFunctionEmitter(const IRFunctionEmitter& src)
			: _pEmitter(src._pEmitter),
			_pfn(src._pfn)
		{
		}

		void IRFunctionEmitter::Init(IREmitter* pEmitter, llvm::Function* pfn)
		{
			assert(pEmitter != nullptr);
			assert(pfn != nullptr);
			_pEmitter = pEmitter;
			_pfn = pfn;
		}

		llvm::Value* IRFunctionEmitter::Call(const std::string& name, llvm::Value* pArg)
		{
			llvm::Function* fn = ResolveFunction(name);
			if (pArg == nullptr)
			{
				return _pEmitter->Call(fn);
			}
			return _pEmitter->Call(fn, pArg);
		}

		llvm::Value* IRFunctionEmitter::Call(const std::string& name, IRValueList& args)
		{
			return _pEmitter->Call(ResolveFunction(name), args);
		}

		llvm::Value* IRFunctionEmitter::Call(const std::string& name, std::initializer_list<llvm::Value*> args)
		{
			_values.init(args);
			return _pEmitter->Call(ResolveFunction(name), _values);
		}

		llvm::Value* IRFunctionEmitter::Call(llvm::Function* pfn, std::initializer_list<llvm::Value*> args)
		{
			assert(pfn != nullptr);
			_values.init(args);
			return _pEmitter->Call(pfn, _values);
		}

		llvm::Value* IRFunctionEmitter::Op(OperatorType  type, llvm::iterator_range<llvm::Function::arg_iterator>& args)
		{
			auto values = args.begin();
			auto l = &(*values);
			++values;
			auto r = &(*values);
			return Op(type, l, r);
		}

		void IRFunctionEmitter::OpV(OperatorType type, size_t count, llvm::Value* pLVal, llvm::Value* pRVal, std::function<void(llvm::Value*, llvm::Value*)> aggregator)
		{
			assert(pLVal != nullptr);
			assert(pRVal != nullptr);

			auto forLoop = ForLoop();
			auto pBodyBlock = forLoop.Begin(count);
			{
				auto i = forLoop.LoadIterationVar();
				llvm::Value* pLItem = ValueAt(pLVal, i);
				llvm::Value* pRItem =  ValueAt(pRVal, i);
				llvm::Value* pTemp = Op(type, pLItem, pRItem);
				aggregator(i, pTemp);
			}
			forLoop.End();
		}

		void IRFunctionEmitter::OpV(OperatorType type, llvm::Value* pCount, llvm::Value* pLVal, llvm::Value* pRVal, std::function<void(llvm::Value*, llvm::Value*)> aggregator)
		{
			assert(pCount != nullptr);
			assert(pLVal != nullptr);
			assert(pRVal != nullptr);

			auto forLoop = ForLoop();
			auto pBodyBlock = forLoop.Begin(pCount);
			{
				auto i = forLoop.LoadIterationVar();
				llvm::Value* pLItem = ValueAt(pLVal, i);
				llvm::Value* pRItem = ValueAt(pRVal, i);
				llvm::Value* pTemp = Op(type, pLItem, pRItem);
				aggregator(i, pTemp);
			}
			forLoop.End();
		}

		void IRFunctionEmitter::OpV(OperatorType type, size_t count, llvm::Value* pLVal, int startAtL, llvm::Value* pRVal, int startAtR, std::function<void(llvm::Value*, llvm::Value*)> aggregator)
		{
			assert(pLVal != nullptr);
			assert(pRVal != nullptr);

			auto forLoop = ForLoop();
			auto pBodyBlock = forLoop.Begin(count);
			{
				auto i = forLoop.LoadIterationVar();

				llvm::Value* lOffset = Op(OperatorType::Add, i, Literal(startAtL));
				llvm::Value* pLItem = ValueAt(pLVal, lOffset);				
				llvm::Value* rOffset = Op(OperatorType::Add, i, Literal(startAtR));
				llvm::Value* pRItem = ValueAt(pRVal, rOffset);
				llvm::Value* pTemp = Op(type, pLItem, pRItem);				
				aggregator(i, pTemp);
			}
			forLoop.End();
		}

		void IRFunctionEmitter::Branch(ComparisonType comparision, llvm::Value* pValue, llvm::Value* pTestValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock)
		{
			llvm::Value* pResult = Cmp(comparision, pValue, pTestValue);
			Branch(pResult, pThenBlock, pElseBlock);
		}

		void IRFunctionEmitter::AppendBlock(llvm::BasicBlock* pBlock)
		{
			assert(pBlock != nullptr);
			_pfn->getBasicBlockList().push_back(pBlock);
		}

		llvm::BasicBlock* IRFunctionEmitter::CurrentBlock(llvm::BasicBlock* pBlock)
		{
			llvm::BasicBlock* pCurrentBlock = CurrentBlock();
			_pEmitter->SetCurrentBlock(pBlock);
			return pCurrentBlock;
		}

		void IRFunctionEmitter::LinkBlocks(llvm::BasicBlock* pTopBlock, llvm::BasicBlock* pBottomBlock)
		{
			assert(pTopBlock != nullptr && pBottomBlock != nullptr);

			auto pTerm = pTopBlock->getTerminator();
			if (pTerm != nullptr)
			{
				pTerm->removeFromParent();
			}
			auto pPrevCurBlock = CurrentBlock(pTopBlock);
			Branch(pBottomBlock);
			CurrentBlock(pPrevCurBlock);
		}

		llvm::Value* IRFunctionEmitter::Store(llvm::Value* pPtr, llvm::Value* pVal)
		{
			return _pEmitter->Store(pPtr, pVal);
		}

		llvm::Value* IRFunctionEmitter::OpAndUpdate(llvm::Value* pPtr, OperatorType op, llvm::Value* pValue)
		{
			llvm::Value* pNextVal = Op(op, Load(pPtr), pValue);
			Store(pPtr, pNextVal);
			return pNextVal;
		}

		llvm::Value* IRFunctionEmitter::PtrOffsetA(llvm::Value* ptr, int offset)
		{
			return _pEmitter->PtrOffset(ptr, Literal(offset));
		}

		llvm::Value* IRFunctionEmitter::PtrOffsetA(llvm::Value* ptr, llvm::Value* pOffset, const std::string& name)
		{
			return _pEmitter->PtrOffset(ptr, pOffset, name);
		}

		llvm::Value* IRFunctionEmitter::ValueAtA(llvm::Value* ptr, int offset)
		{
			return _pEmitter->Load(PtrOffsetA(ptr, offset));
		}

		llvm::Value* IRFunctionEmitter::ValueAtA(llvm::Value* ptr, llvm::Value* pOffset)
		{
			return _pEmitter->Load(PtrOffsetA(ptr, pOffset));
		}

		llvm::Value* IRFunctionEmitter::SetValueAtA(llvm::Value* pPtr, int offset, llvm::Value* pValue)
		{
			return _pEmitter->Store(PtrOffsetA(pPtr, offset), pValue);
		}

		llvm::Value* IRFunctionEmitter::SetValueAtA(llvm::Value* pPtr, llvm::Value* pOffset, llvm::Value* pValue)
		{
			return _pEmitter->Store(PtrOffsetA(pPtr, pOffset), pValue);
		}

		llvm::Value* IRFunctionEmitter::PtrOffsetH(llvm::Value* ptr, int offset)
		{
			return PtrOffsetH(ptr, Literal(offset));
		}

		llvm::Value* IRFunctionEmitter::PtrOffsetH(llvm::Value* ptr, llvm::Value* pOffset)
		{
			assert(ptr != nullptr);
			return _pEmitter->PtrOffset(Load(ptr), pOffset);
		}

		llvm::Value* IRFunctionEmitter::ValueAtH(llvm::Value* ptr, int offset)
		{
			return _pEmitter->Load(PtrOffsetH(ptr, offset));
		}

		llvm::Value* IRFunctionEmitter::ValueAtH(llvm::Value* ptr, llvm::Value* pOffset)
		{
			return _pEmitter->Load(PtrOffsetH(ptr, pOffset));
		}

		llvm::Value* IRFunctionEmitter::SetValueAtH(llvm::Value* pPtr, int offset, llvm::Value* pValue)
		{
			return _pEmitter->Store(PtrOffsetH(pPtr, offset), pValue);
		}

		llvm::Value* IRFunctionEmitter::Ptr(llvm::GlobalVariable* pGlobal)
		{
			return _pEmitter->Ptr(pGlobal);
		}

		llvm::Value* IRFunctionEmitter::PtrOffset(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset)
		{
			return _pEmitter->PtrOffset(pGlobal, pOffset);
		}

		llvm::Value* IRFunctionEmitter::PtrOffset(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset, llvm::Value* pFieldOffset)
		{
			return _pEmitter->PtrOffset(pGlobal, pOffset, pFieldOffset);
		}

		llvm::Value* IRFunctionEmitter::ValueAt(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset)
		{
			return Load(_pEmitter->PtrOffset(pGlobal, pOffset));
		}

		llvm::Value* IRFunctionEmitter::PtrOffset(llvm::Value* pPtr, llvm::Value* pOffset)
		{
			llvm::GlobalVariable* pGlobal = llvm::dyn_cast<llvm::GlobalVariable>(pPtr);
			if (pGlobal != nullptr)
			{
				return PtrOffset(pGlobal, pOffset);
			}
			return PtrOffsetA(pPtr, pOffset);
		}

		llvm::Value* IRFunctionEmitter::PtrOffset(llvm::Value* pPtr, int offset)
		{
			return PtrOffset(pPtr, Literal(offset));
		}

		llvm::Value* IRFunctionEmitter::ValueAt(llvm::Value* pPtr, llvm::Value* pOffset)
		{
			llvm::GlobalVariable* pGlobal = llvm::dyn_cast<llvm::GlobalVariable>(pPtr);
			if (pGlobal != nullptr)
			{
				return ValueAt(pGlobal, pOffset);
			}
			return ValueAtA(pPtr, pOffset);
		}

		llvm::Value* IRFunctionEmitter::ValueAt(llvm::Value* pPtr, int offset)
		{
			return ValueAt(pPtr, Literal(offset));
		}

		llvm::Value* IRFunctionEmitter::SetValueAt(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset, llvm::Value* pVal)
		{
			return Store(PtrOffset(pGlobal, pOffset), pVal);
		}

		llvm::Value* IRFunctionEmitter::SetValueAt(llvm::Value* pPtr, llvm::Value* pOffset, llvm::Value* pVal)
		{
			llvm::GlobalVariable* pGlobal = llvm::dyn_cast<llvm::GlobalVariable>(pPtr);
			if (pGlobal != nullptr)
			{
				return SetValueAt(pGlobal, pOffset, pVal);
			}
			return SetValueAtA(pPtr, pOffset, pVal);
		}

		llvm::Value* IRFunctionEmitter::SetValueAt(llvm::Value* pPtr, int offset, llvm::Value* pVal)
		{
			return SetValueAt(pPtr, Literal(offset), pVal);
		}

		IRForLoopEmitter IRFunctionEmitter::ForLoop()
		{
			return IRForLoopEmitter(*this);
		}
		
		IRIfEmitter IRFunctionEmitter::If()
		{
			return IRIfEmitter(*this);
		}

		llvm::Value* IRFunctionEmitter::Malloc(ValueType type, int64_t size)
		{
			_values.init({ Literal(size) });
			return Cast(Call(MallocFnName, _values), type);
		}

		void IRFunctionEmitter::Free(llvm::Value* pValue)
		{
			Call(FreeFnName, Cast(pValue, ValueType::PByte));
		}

		llvm::Value* IRFunctionEmitter::Print(const std::string& text)
		{
			return Printf({ Literal(text) });
		}

		llvm::Value* IRFunctionEmitter::Printf(std::initializer_list<llvm::Value*> args)
		{
			return Call(PrintfFnName, args);
		}

		void IRFunctionEmitter::PrintForEach(const std::string& formatString, llvm::Value* pVector, int count)
		{
			llvm::Value* pFormat = Literal(formatString);
			IRForLoopEmitter forLoop = ForLoop();
			forLoop.Begin(count);
			{
				auto ival = forLoop.LoadIterationVar();
				auto v = ValueAt(pVector, ival);
				Printf({ pFormat, v });
			}
			forLoop.End();
		}

		llvm::Value* IRFunctionEmitter::DotProductF(int count, llvm::Value* pLVal, llvm::Value* pRVal)
		{
			llvm::Value* pTotal = Var(ValueType::Double);
			DotProductF(count, pLVal, pRVal, pTotal);
			return pTotal;
		}

		void IRFunctionEmitter::DotProductF(int count, llvm::Value* pLVal, llvm::Value* pRVal, llvm::Value* pDest)
		{
			Store(pDest, Literal(0.0));
			OpV(OperatorType::MultiplyF, count, pLVal, pRVal, [&pDest, this](llvm::Value* i, llvm::Value* pValue) {
				OpAndUpdate(pDest, OperatorType::AddF, pValue);
			});
		}

		void IRFunctionEmitter::DotProductF(llvm::Value* pCount, llvm::Value* pLVal, llvm::Value* pRVal, llvm::Value* pDest)
		{
			Store(pDest, Literal(0.0));
			OpV(OperatorType::MultiplyF, pCount, pLVal, pRVal, [&pDest, this](llvm::Value* i, llvm::Value* pValue) {
				OpAndUpdate(pDest, OperatorType::AddF, pValue);
			});
		}

		llvm::Function* IRFunctionEmitter::ResolveFunction(const std::string& name)
		{
			llvm::Function* pfn = Module()->getFunction(name);
			if (pfn == nullptr)
			{
				throw new CompilerException(CompilerError::functionNotFound);
			}
			return pfn;
		}
	}
}
