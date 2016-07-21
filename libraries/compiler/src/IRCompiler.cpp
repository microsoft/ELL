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
			EnsureEmitted(*(node.Var()));
		}

		void IRCompiler::Compile(BinaryNode& node)
		{
			llvm::Value* pVar1 = EnsureEmitted(node.Var1());
			llvm::Value* pVar2 = EnsureEmitted(node.Var2());
			llvm::Value* pDest = EnsureEmitted(node.Result());

			llvm::Value* pResult = _fn.Op(node.Op(), pVar1, pVar2);
			_fn.Store(pDest, pResult);
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

		llvm::Value* IRCompiler::GetEmittedVariable(const VariableScope scope, const std::string& name)
		{
			switch (scope)
			{
				case VariableScope::Literal:
					return _literals.Get(name);
				
				case VariableScope::Global:
					return _globals.Get(name);

				case VariableScope::Local:
					return _locals.Get(name);
				
				default:
					throw new CompilerException(CompilerError::variableScopeNotSupported);
			}
		}

		void IRCompiler::RegisterFunctionArgs(NamedValueTypeList& args)
		{
			auto fnArgs = _fn.Args().begin();
			for (size_t i = 0; i < args.size(); ++i)
			{
				auto arg = &(*fnArgs);
				_locals.Set(args[i].first, arg);
			}
		}

		llvm::Value* IRCompiler::EnsureEmitted(Variable& var)
		{
			llvm::Value* pVal = nullptr;
			if (var.HasEmittedName())
			{
				pVal = GetEmittedVariable(var.Scope(), var.EmittedName());
			}
			if (pVal == nullptr)
			{
				AllocVar(var);
				pVal = GetEmittedVariable(var.Scope(), var.EmittedName());
				if (pVal == nullptr)
				{
					pVal = Emit(var);
				}
			}
			return pVal;
		}

		llvm::Value* IRCompiler::Emit(Variable& var)
		{
			assert(var.HasEmittedName());

			if (var.IsScalar())
			{
				return EmitScalar(var);
			}
			else
			{
				throw new CompilerException(CompilerError::variableTypeNotSupported);
			}
		}

		llvm::Value* IRCompiler::EmitScalar(Variable& var)
		{
			llvm::Value* pVal = nullptr;	
			switch (var.Scope())
			{
				case VariableScope::Literal:
					pVal = EmitLiteral(var);
					break;

				case VariableScope::Local:
					if (var.IsVectorRef())
					{
						pVal = EmitVectorRef(var);
					}
					else
					{
						pVal = EmitLocalScalar(var);
					}
					break;

				case VariableScope::Global:
					pVal = EmitGlobalScalar(var);
					break;

				default:
					throw new CompilerException(CompilerError::variableTypeNotSupported);
			}
			return pVal;
		}

		llvm::Value* IRCompiler::EmitLiteral(Variable& var)
		{
			llvm::Value* pVal = nullptr;
			switch (var.Type())
			{
				case ValueType::Double:
					pVal = EmitLiteral<double>(static_cast<LiteralF&>(var));
					break;
				default:
					throw new CompilerException(CompilerError::valueTypeNotSupported);
			}

			assert(pVal != nullptr);
			_literals.Set(var.EmittedName(), pVal);
			return pVal;
		}

		llvm::Value* IRCompiler::EmitLocalScalar(Variable& var)
		{
			llvm::Value* pVal = nullptr;
			switch (var.Type())
			{
				case ValueType::Double:
					if (var.HasInitValue())
					{
						pVal = EmitLocal<double>(static_cast<InitializedScalarF&>(var));
					}
					else
					{
						pVal = EmitLocal<double>(static_cast<ScalarF&>(var));
					}
					break;
				default:
					throw new CompilerException(CompilerError::valueTypeNotSupported);
			}

			assert(pVal != nullptr);
			_locals.Set(var.EmittedName(), pVal);
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
			_globals.Set(var.EmittedName(), pVal);
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
			_locals.Set(var.EmittedName(), pVal);
			return pVal;
		}

		void IRCompiler::DebugDump()
		{
			_module.Dump();
		}
	}
}
