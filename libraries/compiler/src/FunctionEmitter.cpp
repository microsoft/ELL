#include "FunctionEmitter.h"
#include "LLVMEmitter.h"
#include "EmitterException.h"

namespace emll
{
	namespace compiler
	{
		namespace ir
		{
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

			llvm::Value* FunctionEmitter::Malloc(ValueType type, int64_t size)
			{
				_values.init({ Literal(size) });
				return Cast(Call("malloc", _values), type);
			}

			void FunctionEmitter::Free(llvm::Value* pValue)
			{
				Call("free", Cast(pValue, ValueType::PByte));
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
