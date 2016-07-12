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

		void IRCompiler::BeginMain(const std::string& functionName)
		{
			NamedValueTypeList fnArgs;
			AddArgs(fnArgs, InputName(), Inputs());
			AddArgs(fnArgs, OutputName(), Outputs());
			_fn = _module.Function(functionName, ValueType::Void, fnArgs, true);
		}
		
		void IRCompiler::EndMain()
		{
			_fn.Ret();
			_fn.Verify();
		}

		void IRCompiler::AddArgs(NamedValueTypeList& args, const std::string& namePrefix, const std::vector<const model::Node*>& nodes)
		{
			for (size_t n = 0; n < nodes.size(); ++n)
			{
				auto node = nodes[n];
				std::string argNamePrefix = namePrefix;
				argNamePrefix.append(std::to_string(n));
				auto outputs = node->GetOutputs();
				for (size_t i = 0; i < outputs.size(); ++i)
				{
					std::string argName = argNamePrefix;
					if (i > 0)
					{
						argName.append(std::to_string(i));
					}
					AddArgs(args, argName, outputs[i]);
				}
			}
		}

		void IRCompiler::AddArgs(NamedValueTypeList& args, const std::string& name, const model::OutputPortBase* pOutput)
		{
			model::Port::PortType type = pOutput->GetType();
			switch (type)
			{
				case model::Port::PortType::Real:
					args.push_back({name, ValueType::PDouble});
					break;
				case model::Port::PortType::Integer:
					args.push_back({ name, ValueType::PInt32 });
					break;
				default:
					throw new CompilerException(CompilerError::inputPortTypeNotSupported);
			}
		}
	}
}
