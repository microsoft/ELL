#include "IRCompiler.h"
#include <stdio.h>

namespace emll
{
	namespace compiler
	{
		IRCompiler::IRCompiler(const std::string& moduleName, std::ostream& os)
			:  _module(_emitter, moduleName), _os(os)
		{
		}

		void IRCompiler::DebugDump()
		{
			_module.Dump();
		}

		void IRCompiler::Begin()
		{
		}

		void IRCompiler::End()
		{

		}

		void IRCompiler::BeginFunction(const std::string& functionName, const VariableDecl& input, const VariableDecl& output)
		{
			NamedValueTypeList fnArgs;
			fnArgs.init({ { input.name, input.type },{ output.name, output.type } });
			_fn = _module.Function(functionName, ValueType::Void, fnArgs, true);
		}

		void IRCompiler::EndFunction()
		{
			_fn.Ret();
			_fn.Verify();
		}

	}
}
