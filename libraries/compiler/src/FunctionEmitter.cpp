#include "FunctionEmitter.h"
#include "LLVMEmitter.h"
#include "EmitterException.h"

namespace emll
{
	namespace compiler
	{
		namespace ir
		{
			const std::string LoopInitBlockName = "loop.init";
			const std::string LoopConditionBlockName = "loop.cond";
			const std::string LoopBodyBlockName = "loop.body";
			const std::string LoopIncBlockName = "loop.inc";
			const std::string LoopAfterBlockName = "loop.after";
			const std::string LoopIncVar = "i";

			const std::string PrintfFnName = "printf";
			const std::string MallocFnName = "malloc";
			const std::string FreeFnName = "free";

			FunctionEmitter::FunctionEmitter(LLVMEmitter* pEmitter, llvm::Function* pfn)
				: _pEmitter(pEmitter),
				_pfn(pfn)
			{
			}

			FunctionEmitter::FunctionEmitter(const FunctionEmitter& src)
				: _pEmitter(src._pEmitter),
				_pfn(src._pfn)
			{
			}

			void FunctionEmitter::AddBlock(llvm::BasicBlock* pBlock)
			{
				assert(pBlock != nullptr);
				_pfn->getBasicBlockList().push_back(pBlock);
			}

			llvm::BasicBlock* FunctionEmitter::CurrentBlock(llvm::BasicBlock* pBlock)
			{
				llvm::BasicBlock* pCurrentBlock = CurrentBlock();
				_pEmitter->SetCurrentBlock(pBlock);
				return pCurrentBlock;
			}

			llvm::Value* FunctionEmitter::Call(const std::string& name, llvm::Value* pArg)
			{
				llvm::Function* fn = ResolveFunction(name);
				if (pArg == nullptr)
				{
					return _pEmitter->Call(fn);
				}
				return _pEmitter->Call(fn, pArg);
			}

			llvm::Value* FunctionEmitter::Call(const std::string& name, ValueList& args)
			{
				return _pEmitter->Call(ResolveFunction(name), args);
			}

			llvm::Value* FunctionEmitter::Call(const std::string& name, std::initializer_list<llvm::Value*> args)
			{
				_values.init(args);
				return _pEmitter->Call(ResolveFunction(name), _values);
			}

			llvm::Value* FunctionEmitter::Op(OperatorType  type, llvm::iterator_range<llvm::Function::arg_iterator>& args)
			{
				auto values = args.begin();
				auto l = &(*values);
				++values;
				auto r = &(*values);
				return Op(type, l, r);
			}

			llvm::Value* FunctionEmitter::Store(llvm::Value* pPtr, llvm::Value* pVal)
			{
				return _pEmitter->Store(pPtr, pVal);
			}

			llvm::Value* FunctionEmitter::OpAndUpdate(llvm::Value* pPtr, OperatorType op, llvm::Value* pValue)
			{
				llvm::Value* pNextVal = Op(op, Load(pPtr), pValue);
				Store(pPtr, pNextVal);
				return pNextVal;
			}

			llvm::Value* FunctionEmitter::PtrOffsetA(llvm::Value* ptr, int offset)
			{
				return _pEmitter->ArrayDeref(ptr, Literal(offset));
			}

			llvm::Value* FunctionEmitter::ValueAtA(llvm::Value* ptr, int offset)
			{
				return _pEmitter->Load(PtrOffsetA(ptr, offset));
			}

			llvm::Value* FunctionEmitter::SetValueAtA(llvm::Value* pPtr, int offset, llvm::Value* pValue)
			{
				return _pEmitter->Store(PtrOffsetA(pPtr, offset), pValue);
			}

			llvm::Value* FunctionEmitter::PtrOffsetH(llvm::Value* ptr, int offset)
			{
				return _pEmitter->ArrayDeref(Load(ptr), Literal(offset));
			}

			llvm::Value* FunctionEmitter::ValueAtH(llvm::Value* ptr, int offset)
			{
				return _pEmitter->Load(PtrOffsetH(ptr, offset));
			}

			llvm::Value* FunctionEmitter::SetValueAtH(llvm::Value* pPtr, int offset, llvm::Value* pValue)
			{
				return _pEmitter->Store(PtrOffsetH(pPtr, offset), pValue);
			}

			ForLoopEmitter FunctionEmitter::ForLoop()
			{
				return ForLoopEmitter(*this);
			}

			llvm::Value* FunctionEmitter::Malloc(ValueType type, int64_t size)
			{
				_values.init({ Literal(size) });
				return Cast(Call(MallocFnName, _values), type);
			}

			void FunctionEmitter::Free(llvm::Value* pValue)
			{
				Call(FreeFnName, Cast(pValue, ValueType::PByte));
			}

			llvm::Value* FunctionEmitter::Printf(std::initializer_list<llvm::Value*> args)
			{
				return Call(PrintfFnName, args);
			}

			llvm::Function* FunctionEmitter::ResolveFunction(const std::string& name)
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
}
