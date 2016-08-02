#include "IRRuntime.h"

namespace emll
{
	namespace compiler
	{
		static const std::string& c_CountName = "count";
		static const std::string& c_LVectorName = "pLVector";
		static const std::string& c_RVectorName = "pRVector";
		static const std::string& c_ResultName = "pResult";

		static const std::string& c_DotProductFnName = "Emll_DotProductF";

		IRRuntime::IRRuntime(IRModuleEmitter& module)
			: _module(module)
		{
		}

		llvm::Function* IRRuntime::DotProductF()
		{
			if (_pfnDotProductF == nullptr)
			{
				_pfnDotProductF = EmitDotProductFunctionF();
			}
			return _pfnDotProductF;
		}

		llvm::Function* IRRuntime::EmitDotProductFunctionF()
		{
			auto curBlock = _module.Emitter().CurrentBlock();

			_args.init({
				{c_CountName, ValueType::Int32},
				{c_LVectorName, ValueType::PDouble},
				{c_RVectorName, ValueType::PDouble},
				{c_ResultName, ValueType::PDouble}
			});
			auto fn = _module.Function(c_DotProductFnName, ValueType::Void, _args);
			auto args = fn.Args().begin();
			llvm::Argument& count = *args++;
			llvm::Argument& lVal = *args++;
			llvm::Argument& rVal = *args++;
			llvm::Argument& result = *args++;
			fn.DotProductF(&count, &lVal, &rVal, &result);
			fn.Ret();
			fn.Verify();

			_module.Emitter().SetCurrentBlock(curBlock);

			return fn.Function();
		}
	}
}