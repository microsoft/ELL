////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CppCompiler.cpp (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CppCompiler.h"
#include "Files.h"

namespace emll
{
	namespace compiler
	{
		static const std::string c_LoopVarName = "i";

		CppCompiler::CppCompiler()
		{
		}

		void CppCompiler::WriteToFile(const std::string& filePath)
		{
			auto fileStream = utilities::OpenOfstream(filePath);
			_module.Write(fileStream);
		}

		const model::Node* CppCompiler::GetUniqueParent(const model::Node& node)
		{
			auto inputs = node.GetInputPorts();
			const model::Node* pParentNode = nullptr;
			CppBlock* pParentBlock = nullptr;
			for (auto input : inputs)
			{
				for (auto parentNode : input->GetParentNodes())
				{
					CppBlock* pNodeBlock = nullptr;
					auto result = _nodeBlocks.find(parentNode->GetId());
					if (result != _nodeBlocks.end())
					{
						pNodeBlock = result->second;
					}
					if (pNodeBlock != nullptr)
					{
						if (pParentBlock != nullptr && pParentBlock != pNodeBlock)
						{
							return nullptr;
						}
						pParentBlock = pNodeBlock;
						pParentNode = parentNode;
					}
				}
			}
			return pParentNode;
		}

		void CppCompiler::BeginCodeBlock(const model::Node& node) 
		{
			CppBlock* pBlock = _pfn->AppendBlock();
			pBlock->Comment(node.GetRuntimeTypeName());
			_nodeBlocks[node.GetId()] = pBlock;
		}

		void CppCompiler::EndCodeBlock(const model::Node& node)
		{
			auto pBlock = _nodeBlocks.at(node.GetId());
			assert(pBlock != nullptr);

			const model::Node* pParentNode = GetUniqueParent(node);
			if (pParentNode == nullptr)
			{
				std::string comment = "Block" + std::to_string(pBlock->Id());
				_pfn->Comment(comment);
				return;
			}
			auto pParentBlock = _nodeBlocks.at(pParentNode->GetId());
			assert(pParentBlock != nullptr);
			_pfn->MergeBlocks(pParentBlock, pBlock);
			_nodeBlocks[node.GetId()] = pParentBlock;
		}

		void CppCompiler::CompileDotProductNode(const model::Node& node)
		{
			throw new CompilerException(CompilerError::notSupported);
		}
		void CppCompiler::CompileAccumulatorNode(const model::Node& node)
		{
			throw new CompilerException(CompilerError::notSupported);
		}
		void CppCompiler::CompileDelayNode(const model::Node& node)
		{
			throw new CompilerException(CompilerError::notSupported);
		}

		void CppCompiler::CompileUnaryNode(const model::Node& node)
		{
			throw new CompilerException(CompilerError::notSupported);
		}

		void CppCompiler::CompileElementSelectorNode(const model::Node& node)
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

		void CppCompiler::BeginFunction(const std::string& functionName, NamedValueTypeList& args)
		{
			_pfn = _module.Function(functionName, ValueType::Void, args);
		}
		void CppCompiler::EndFunction()
		{
			if (_pfn != nullptr)
			{
				_pfn->End();
				_nodeBlocks.clear();
			}
		}

		void CppCompiler::EnsureEmitted(Variable& var)
		{
			if (var.HasEmittedName())
			{
				return;
			}
			AllocVar(var);
			if (var.IsNew())
			{
				Emit(var);
			}
		}

		Variable* CppCompiler::EnsureEmitted(model::OutputPortBase* pPort)
		{
			assert(pPort != nullptr);
			Variable* pVar = GetVariableFor(pPort);
			if (pVar == nullptr)
			{
				pVar = AllocVar(pPort);
			}
			assert(pVar != nullptr);
			EnsureEmitted(pVar);
			return pVar;
		}

		Variable* CppCompiler::EnsureEmitted(model::OutputPortElement elt)
		{
			Variable* pVar = EnsureVariableFor(elt);
			EnsureEmitted(pVar);
			return pVar;
		}

		Variable* CppCompiler::EnsureEmitted(model::InputPortBase* pPort)
		{
			assert(pPort != nullptr);
			return EnsureEmitted(pPort->GetOutputPortElement(0));
		}

		void CppCompiler::Emit(Variable& var)
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

		Variable* CppCompiler::LoadVar(const model::OutputPortElement elt)
		{
			Variable* pVar = EnsureVariableFor(elt);
			EnsureEmitted(pVar);
			_pfn->Value(*pVar, elt.GetIndex());
			return pVar;
		}

		Variable* CppCompiler::LoadVar(model::InputPortBase* pPort)
		{
			assert(pPort != nullptr);
			return LoadVar(pPort->GetOutputPortElement(0));
		}

		const std::string& CppCompiler::LoopVarName()
		{
			return c_LoopVarName;
		}
	}
}
