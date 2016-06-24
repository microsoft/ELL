#include "IRFunctionEmitter.h"
#include "IREmitter.h"
#include "EmitterException.h"

namespace emll
{
	namespace compiler
	{
		const std::string PrintfFnName = "printf";
		const std::string MallocFnName = "malloc";
		const std::string FreeFnName = "free";

		IRFunctionEmitter::IRFunctionEmitter(IREmitter* pEmitter, llvm::Function* pfn)
			: _pEmitter(pEmitter),
			_pfn(pfn)
		{
		}

		IRFunctionEmitter::IRFunctionEmitter(const IRFunctionEmitter& src)
			: _pEmitter(src._pEmitter),
			_pfn(src._pfn)
		{
		}

		void IRFunctionEmitter::AddBlock(llvm::BasicBlock* pBlock)
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

		llvm::Value* IRFunctionEmitter::Op(OperatorType  type, llvm::iterator_range<llvm::Function::arg_iterator>& args)
		{
			auto values = args.begin();
			auto l = &(*values);
			++values;
			auto r = &(*values);
			return Op(type, l, r);
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
			return _pEmitter->ArrayDeref(ptr, Literal(offset));
		}

		llvm::Value* IRFunctionEmitter::ValueAtA(llvm::Value* ptr, int offset)
		{
			return _pEmitter->Load(PtrOffsetA(ptr, offset));
		}

		llvm::Value* IRFunctionEmitter::SetValueAtA(llvm::Value* pPtr, int offset, llvm::Value* pValue)
		{
			return _pEmitter->Store(PtrOffsetA(pPtr, offset), pValue);
		}

		llvm::Value* IRFunctionEmitter::PtrOffsetH(llvm::Value* ptr, int offset)
		{
			return _pEmitter->ArrayDeref(Load(ptr), Literal(offset));
		}

		llvm::Value* IRFunctionEmitter::ValueAtH(llvm::Value* ptr, int offset)
		{
			return _pEmitter->Load(PtrOffsetH(ptr, offset));
		}

		llvm::Value* IRFunctionEmitter::SetValueAtH(llvm::Value* pPtr, int offset, llvm::Value* pValue)
		{
			return _pEmitter->Store(PtrOffsetH(pPtr, offset), pValue);
		}

		IRForLoopEmitter IRFunctionEmitter::ForLoop()
		{
			return IRForLoopEmitter(*this);
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

		llvm::Value* IRFunctionEmitter::Printf(std::initializer_list<llvm::Value*> args)
		{
			return Call(PrintfFnName, args);
		}

		llvm::Function* IRFunctionEmitter::ResolveFunction(const std::string& name)
		{
			llvm::Function* pfn = Module()->getFunction(name);
			if (pfn == nullptr)
			{
				throw new EmitterException(EmitterError::FunctionNotFound);
			}
			return pfn;
		}
	}
}
