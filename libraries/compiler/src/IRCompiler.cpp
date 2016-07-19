#include "IRCompiler.h"
#include "ScalarVar.h"
#include <stdio.h>

namespace emll
{
	namespace compiler
	{
		IRCompiler::IRCompiler(const std::string& moduleName, std::ostream& os)
			:  _module(_emitter, moduleName), _os(os)
		{
		}

		void IRCompiler::Compile(LiteralNode& node)
		{
			EnsureVariable(*(node.Var()));
		}

		llvm::Value* IRCompiler::GetVariable(const std::string& name)
		{
			return _vars.Get(name);
		}

		llvm::Value* IRCompiler::EnsureVariable(Variable& var)
		{
			llvm::Value* pVal = nullptr;
			if (!var.HasEmittedName())
			{
				if (var.IsScalar())
				{
					pVal = EmitScalar(var);
				}
				else
				{
					throw new CompilerException(CompilerError::variableTypeNotSupported);
				}
				_vars.Set(var.EmittedName(), pVal);
			}
			else
			{
				pVal = _vars.Get(var.EmittedName());
			}
			assert(pVal != nullptr);
			return pVal;
		}

		llvm::Value* IRCompiler::EmitScalar(Variable& var)
		{
			llvm::Value* pVal = nullptr;	
			switch (var.Scope())
			{
				case VariableScope::Global:
					pVal = EmitGlobalScalar(var);
					break;

				case VariableScope::Local:
					pVal = EmitLocalScalar(var);
					break;

				default:
					throw new CompilerException(CompilerError::variableTypeNotSupported);
			}
			return pVal;
		}

		llvm::Value* IRCompiler::EmitLocalScalar(Variable& var)
		{
			llvm::Value* pVal = nullptr;
			switch (var.Type())
			{
				case ValueType::Double:
					pVal = EmitLocalScalar<double>(static_cast<LocalScalarF&>(var));
					break;
				default:
					throw new CompilerException(CompilerError::valueTypeNotSupported);
			}

			assert(pVal != nullptr);
			return pVal;
		}

		llvm::Value* IRCompiler::EmitGlobalScalar(Variable& var)
		{
			llvm::Value* pVal = nullptr;
			switch (var.Type())
			{
			case ValueType::Double:
				pVal = EmitGlobalScalar<double>(static_cast<GlobalScalarF&>(var));
				break;
			default:
				throw new CompilerException(CompilerError::valueTypeNotSupported);
			}
			assert(pVal != nullptr);
			return pVal;
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
				std::string argNamePrefix = MakeVarName(namePrefix, n);
				auto outputs = node->GetOutputPorts();
				for (size_t i = 0; i < outputs.size(); ++i)
				{
					std::string argName;
					if (i > 0)
					{
						argName = MakeVarName(argName, i);
					}
					else
					{
						argName = argNamePrefix;
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

		std::string IRCompiler::MakeVarName(const std::string& namePrefix, size_t i)
		{
			std::string name = namePrefix;
			name.append(std::to_string(i));
			return name;
		}
	}
}
