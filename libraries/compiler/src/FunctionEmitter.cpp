#include "FunctionEmitter.h"
#include "LLVMEmitter.h"
#include "EmitterException.h"

namespace emll
{
	namespace compiler
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

		llvm::Value* FunctionEmitter::call(const std::string& name, llvm::Value* pArg)
		{
			llvm::Function* fn = resolveFunction(name);
			if (pArg == nullptr)
			{
				return _pEmitter->addCall(fn);
			}
			return _pEmitter->addCall(fn, pArg);
		}

		llvm::Value* FunctionEmitter::call(const std::string& name, ValueList& args)
		{
			return _pEmitter->addCall(resolveFunction(name), args);
		}

		llvm::Value* FunctionEmitter::call(const std::string& name, std::initializer_list<llvm::Value*> args)
		{
			_values.init(args);
			return _pEmitter->addCall(resolveFunction(name), _values);
		}

		llvm::Value* FunctionEmitter::op(OperatorType type, llvm::iterator_range<llvm::Function::arg_iterator>& args)
		{
			auto values = args.begin();
			auto l = &(*values);
			++values;
			auto r = &(*values);
			return op(type, l, r);
		}

		llvm::Value* FunctionEmitter::store(llvm::Value* pPtr, llvm::Value* pVal)
		{
			return _pEmitter->addStore(pPtr, pVal);
		}

		llvm::Value* FunctionEmitter::ptrOffsetA(llvm::Value* ptr, int offset)
		{
			return _pEmitter->addArrayDeref(ptr, literal(offset));
		}

		llvm::Value* FunctionEmitter::valueAtA(llvm::Value* ptr, int offset)
		{
			return _pEmitter->addLoad(ptrOffsetA(ptr, offset));
		}

		llvm::Value* FunctionEmitter::setValueAtA(llvm::Value* pPtr, int offset, llvm::Value* pValue)
		{
			return _pEmitter->addStore(ptrOffsetA(pPtr, offset), pValue);
		}

		llvm::Value* FunctionEmitter::ptrOffsetH(llvm::Value* ptr, int offset)
		{
			return _pEmitter->addArrayDeref(load(ptr), literal(offset));
		}

		llvm::Value* FunctionEmitter::valueAtH(llvm::Value* ptr, int offset)
		{
			return _pEmitter->addLoad(ptrOffsetH(ptr, offset));
		}

		llvm::Value* FunctionEmitter::setValueAtH(llvm::Value* pPtr, int offset, llvm::Value* pValue)
		{
			return _pEmitter->addStore(ptrOffsetH(pPtr, offset), pValue);
		}

		llvm::Value* FunctionEmitter::malloc(ValueType type, int64_t size)
		{
			_values.init({ literal(size) });
			return cast(call("malloc", _values), type);
		}

		void FunctionEmitter::free(llvm::Value* pValue)
		{
			call("free", cast(pValue, ValueType::PByte));
		}

		llvm::Function* FunctionEmitter::resolveFunction(const std::string& name)
		{
			llvm::Function* pfn = module()->getFunction(name);
			if (pfn == nullptr)
			{
				throw new EmitterException(EmitterError::FunctionNotFound);
			}
			return pfn;
		}
	}
}
