////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IRCompiler.cpp (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRCompiler.h"
#include "ScalarVar.h"
#include "VectorVar.h"

#include "Files.h"

#include <stdio.h>

namespace emll
{
	namespace compiler
	{
		IRCompiler::IRCompiler()
			: IRCompiler("EMLL")
		{
		}

		IRCompiler::IRCompiler(const std::string& moduleName)
			:  _module(_emitter, moduleName), _runtime(_module)
		{
		}
		
		void IRCompiler::CompileDotProductNode(const model::Node& node)
		{
			switch (ModelEx::GetNodeDataType(node))
			{
				case model::Port::PortType::Real:
					CompileDotProduct<double>(static_cast<const nodes::DotProductNode<double>&>(node));
					break;
				default:
					throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void IRCompiler::CompileAccumulatorNode(const model::Node& node)
		{
			switch (ModelEx::GetNodeDataType(node))
			{
				case model::Port::PortType::Real:
					CompileAccumulator<double>(static_cast<const nodes::AccumulatorNode<double>&>(node));
					break;
				case model::Port::PortType::Integer:
					CompileAccumulator<int>(static_cast<const nodes::AccumulatorNode<int>&>(node));
					break;
				default:
					throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void IRCompiler::CompileDelayNode(const model::Node& node)
		{
			switch (ModelEx::GetNodeDataType(node))
			{
				case model::Port::PortType::Real:
					CompileDelay<double>(static_cast<const nodes::DelayNode<double>&>(node));
					break;
				case model::Port::PortType::Integer:
					CompileDelay<int>(static_cast<const nodes::DelayNode<int>&>(node));
					break;
				default:
					throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void IRCompiler::CompileUnaryNode(const model::Node& node)
		{
			switch (ModelEx::GetNodeDataType(node))
			{
				case model::Port::PortType::Real:
					CompileUnary<double>(static_cast<const nodes::UnaryOperationNode<double>&>(node));
					break;
				case model::Port::PortType::Integer:
					CompileUnary<int>(static_cast<const nodes::UnaryOperationNode<int>&>(node));
					break;
				default:
					throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void IRCompiler::CompileBinaryPredicateNode(const model::Node& node)
		{
			auto input = node.GetInputPorts()[0];
			switch (input->GetType())
			{
				case model::Port::PortType::Real:
					CompileBinaryPredicate<double>(static_cast<const nodes::BinaryPredicateNode<double>&>(node));
					break;
				default:
					throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void IRCompiler::CompileElementSelectorNode(const model::Node& node)
		{
			switch (ModelEx::GetNodeDataType(node))
			{
				case model::Port::PortType::Real:
					CompileElementSelectorNode<double>(node);
					break;
				default:
					throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void IRCompiler::BeginFunction(const std::string& functionName, NamedValueTypeList& args)
		{			
			EndFunction();
			
			_locals.Clear();  // Only locals can be reused. 

			_fn = _module.Function(functionName, ValueType::Void, args, true);
			RegisterFunctionArgs(args);
		}

		void IRCompiler::EndFunction()
		{
			if (_fn.Function() != nullptr)
			{
				_fn.Ret();
				_fn.Verify();
			}
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

		llvm::Value* IRCompiler::EnsureEmitted(model::OutputPortElement elt)
		{
			return EnsureEmitted(EnsureVariableFor(elt));
		}

		llvm::Value* IRCompiler::EnsureEmitted(model::OutputPortBase* pPort)
		{
			assert(pPort != nullptr);
			Variable* pVar = GetVariableFor(pPort);
			if (pVar == nullptr)
			{
				pVar = AllocVar(pPort);
			}
			assert(pVar != nullptr);
			return EnsureEmitted(pVar);
		}

		llvm::Value* IRCompiler::EnsureEmitted(model::InputPortBase* pPort)
		{
			assert(pPort != nullptr);
			return EnsureEmitted(pPort->GetOutputPortElement(0));
		}

		llvm::Value* IRCompiler::Emit(Variable& var)
		{
			assert(var.HasEmittedName());
			switch (var.Type())
			{
				case ValueType::Double:
					return Emit<double>(var);
				case ValueType::Byte:
					return Emit<uint8_t>(var);
				case ValueType::Int32:
					return Emit<int>(var);
				case ValueType::Int64:
					return Emit<int64_t>(var);
				default:
					break;
			}
			throw new CompilerException(CompilerError::variableTypeNotSupported);
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

		llvm::Value* IRCompiler::LoadVar(const model::OutputPortElement elt)
		{
			Variable* pVar = EnsureVariableFor(elt);
			llvm::Value* pVal = EnsureEmitted(pVar);
			if (pVar->IsScalar())
			{
				if (elt.GetIndex() > 0)
				{
					throw new CompilerException(CompilerError::vectorVariableExpected);
				}
				if (pVar->IsLiteral())
				{
					return pVal;
				}
				return _fn.Load(pVal);
			}

			if (elt.GetIndex() >= pVar->Dimension())
			{
				throw new CompilerException(CompilerError::indexOutOfRange);
			}
			return _fn.ValueAt(pVal, _fn.Literal((int) elt.GetIndex()));
		}
		
		llvm::Value* IRCompiler::LoadVar(model::InputPortBase* pPort)
		{
			assert(pPort != nullptr);
			return LoadVar(pPort->GetOutputPortElement(0));
		}

		void IRCompiler::SetVar(Variable& var, llvm::Value* pDest, int offset, llvm::Value* pValue)
		{
			assert(pValue != nullptr);
			if (var.IsScalar())
			{
				if (offset > 0)
				{
					throw new CompilerException(CompilerError::indexOutOfRange);
				}
				_fn.Store(pDest, pValue);
				return;
			}
			if (offset >= var.Dimension())
			{
				throw new CompilerException(CompilerError::indexOutOfRange);
			}
			_fn.SetValueAt(pDest, _fn.Literal(offset), pValue);
		}

		void IRCompiler::DebugDump()
		{
			_module.Dump();
		}

		void IRCompiler::WriteAsmToFile(const std::string& filePath)
		{
			_module.WriteAsmToFile(filePath);
		}

		void IRCompiler::WriteBitcodeToFile(const std::string& filePath)
		{
			_module.WriteBitcodeToFile(filePath);
		}
	}
}
