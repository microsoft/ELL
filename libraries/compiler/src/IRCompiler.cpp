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

		void IRCompiler::Begin()
		{
		}

		void IRCompiler::End()
		{

		}

		void IRCompiler::BeginMain(const std::string& functionName)
		{
			NamedValueTypeList fnArgs;
			fnArgs.init({ { InputName(), ValueType::PDouble },{ OutputName(), ValueType::PDouble } });
			BeginMain(functionName, fnArgs);
		}

		void IRCompiler::BeginMain(const std::string& functionName, NamedValueTypeList& args)
		{			
			_fn = _module.Function(functionName, ValueType::Void, args, true);
			RegisterFunctionArgs(args);
		}

		void IRCompiler::EndMain()
		{
			_fn.Ret();
			_fn.Verify();
		}

		llvm::Value* IRCompiler::GetVariable(const std::string& name)
		{
			return _vars.Get(name);
		}

		llvm::Value* IRCompiler::EnsureVariable(Variable& var)
		{
			llvm::Value* pVal = nullptr;
			if (var.HasEmittedName())
			{
				pVal = _vars.Get(var.EmittedName());
			}
			if (pVal == nullptr)
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
			return pVal;
		}

		void IRCompiler::RegisterFunctionArgs(NamedValueTypeList& args)
		{
			auto fnArgs = _fn.Args().begin();
			for (size_t i = 0; i < args.size(); ++i)
			{
				auto arg = &(*fnArgs);
				_vars.Set(args[i].first, arg);
			}
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
					if (var.IsVectorRef())
					{
						pVal = EmitVectorRef(var);
					}
					else
					{
						pVal = EmitLocal<double>(static_cast<InitializedScalarF&>(var));
					}
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
					pVal = EmitGlobal<double>(static_cast<InitializedScalarF&>(var));
					break;
				default:
					throw new CompilerException(CompilerError::valueTypeNotSupported);
			}
			assert(pVal != nullptr);
			return pVal;
		}

		llvm::Value* IRCompiler::EmitVectorRef(Variable& var)
		{
			llvm::Value* pVal = nullptr;
			switch (var.Type())
			{
			case ValueType::Double:
				pVal = EmitRef<double>(static_cast<VectorRefScalarVarF&>(var));
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
	}
}
