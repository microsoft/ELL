#include "IRCompiler.h"
#include "ScalarVar.h"
#include "VectorVar.h"
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

		void IRCompiler::Compile(InputNode& node)
		{
			EnsureEmitted(*(node.Var()));
		}

		void IRCompiler::Compile(OutputNode& node)
		{
			Variable& destVar = *(node.DestVar());
			llvm::Value* pDest = EnsureEmitted(destVar);
			llvm::Value* pResult = LoadVar(*(node.Var()));
			if (node.ElementIndex() >= 0)
			{
				assert(destVar.IsVector());
				_fn.SetValueAtA(pDest, node.ElementIndex(), pResult);
			}
			else
			{
				assert(destVar.IsScalar());
				_fn.Store(pDest, pResult);
			}
		}

		void IRCompiler::Compile(BinaryNode& node)
		{
			llvm::Value* pSrc1 = LoadVar(*(node.Src1()));
			llvm::Value* pSrc2 = LoadVar(*(node.Src2()));
			llvm::Value* pDest = EnsureEmitted(*(node.Var()));
			llvm::Value* pResult = _fn.Op(node.Op(), pSrc1, pSrc2);
			_fn.Store(pDest, pResult);
		}

		void IRCompiler::Compile(DotProductNode& node)
		{
			llvm::Value* pSrc1 = LoadVar(node.Src1());
			llvm::Value* pSrc2 = EnsureEmitted(node.Src2());
			llvm::Value* pDest = EnsureEmitted(node.Var());
			_fn.DotProductF(node.Var()->Dimension(), pSrc1, pSrc2, pDest);
		}

		void IRCompiler::BeginFunction(const std::string& functionName, NamedValueTypeList& args)
		{			
			_fn = _module.Function(functionName, ValueType::Void, args, true);
			RegisterFunctionArgs(args);
		}

		void IRCompiler::BeginFunction(const std::string& functionName, DataFlowGraph& graph)
		{
			NamedValueTypeList fnArgs;
			AddFunctionArgs(graph, fnArgs);
			BeginFunction(functionName, fnArgs);
		}

		void IRCompiler::EndFunction()
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
				case VariableScope::Input:
				case VariableScope::Output:
					return _locals.Get(name);
				
				default:
					throw new CompilerException(CompilerError::variableScopeNotSupported);
			}
		}

		void IRCompiler::AddFunctionArgs(DataFlowGraph& graph, NamedValueTypeList& args)
		{
			AddFunctionArgs(graph.InputArgs(), args);
			AddFunctionArgs(graph.OutputArgs(), args);
		}

		void IRCompiler::AddFunctionArgs(const std::vector<ArgNode*>& argNodes, NamedValueTypeList& fnArgs)
		{
			for (ArgNode* pNode : argNodes)
			{
				Variable& var = *(pNode->Var());
				AllocVar(var);
				fnArgs.emplace_back(var.EmittedName(), GetPtrType(var.Type()));
			}
		}

		void IRCompiler::RegisterFunctionArgs(NamedValueTypeList& args)
		{
			auto fnArgs = _fn.Args().begin();
			for (size_t i = 0; i < args.size(); ++i)
			{
				auto arg = &(*fnArgs);
				_locals.Set(args[i].first, arg);
				++fnArgs;
			}
		}

		llvm::Value* IRCompiler::EnsureEmitted(Variable* pVar)
		{
			assert(pVar != nullptr);
			return EnsureEmitted(*pVar);
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
			if (var.IsComputed())
			{
				ApplyComputed(var, pVal);
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
			else if (var.IsVector())
			{
				return EmitVector(var);
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

		void IRCompiler::ApplyComputed(Variable& var, llvm::Value* pDest)
		{
			llvm::Value* pVal = nullptr;
			switch (var.Type())
			{
				case ValueType::Double:
					ApplyComputed<double>(static_cast<ComputedVar<double>&>(var), pDest);
					break;
				default:
					throw new CompilerException(CompilerError::valueTypeNotSupported);
			}
		}

		llvm::Value* IRCompiler::EmitVector(Variable& var)
		{
			llvm::Value* pVal = nullptr;
			switch (var.Scope())
			{
				case VariableScope::Literal:
					pVal = EmitLiteralV(var);
					break;

				default:
					throw new CompilerException(CompilerError::variableTypeNotSupported);
			}
			return pVal;
		}

		llvm::Value* IRCompiler::EmitLiteralV(Variable& var)
		{
			llvm::Value* pVal = nullptr;
			switch (var.Type())
			{
				case ValueType::Double:
					pVal = EmitLiteralV<double>(static_cast<LiteralVarV<double>&>(var));
					break;
				default:
					throw new CompilerException(CompilerError::valueTypeNotSupported);
			}
			return pVal;
		}

		llvm::Value* IRCompiler::LoadVar(Variable* pVar)
		{
			assert(pVar != nullptr);
			return LoadVar(*pVar);
		}

		llvm::Value* IRCompiler::LoadVar(Variable& var)
		{
			llvm::Value* pVal = EnsureEmitted(var);
			if (!var.IsLiteral())
			{
				pVal = _fn.Load(pVal);
			}
			return pVal;
		}

		void IRCompiler::DebugDump()
		{
			_module.Dump();
		}
	}
}
