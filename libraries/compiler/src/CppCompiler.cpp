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
					if (!ModelEx::HasSingleDescendant(*parentNode))
					{
						return nullptr;
					}
					CppBlock* pNodeBlock = _nodeBlocks.Get(parentNode);
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

		void CppCompiler::NewCodeBlock(const model::Node& node) 
		{
			CppBlock* pBlock = _pfn->AppendBlock();
			_nodeBlocks.Set(node, pBlock);
			//_pfn->Comment(node.GetRuntimeTypeName());
			//_pfn->Comment(_pfn->CurrentBlock()->IdString());
		}

		bool CppCompiler::TryMergeCodeBlock(const model::Node& node)
		{
			auto pBlock = _nodeBlocks.Get(node);
			if (pBlock == nullptr)
			{
				return false;
			}

			const model::Node* pParentNode = GetUniqueParent(node);
			if (pParentNode == nullptr)
			{
				return false;
			}

			return TryMergeNodeBlocks(*pParentNode, node);
		}

		bool CppCompiler::TryMergeNodeBlocks(const model::Node& dest, const model::Node& src)
		{
			CppBlock* pDestBlock = _nodeBlocks.Get(dest);
			if (pDestBlock == nullptr)
			{
				return false;
			}
			return TryMergeNodeIntoBlock(pDestBlock, src);
		}

		bool CppCompiler::TryMergeNodeIntoBlock(CppBlock* pDestBlock, const model::Node& src)
		{
			CppBlock* pSrcBlock = _nodeBlocks.Get(src);
			if (pSrcBlock == nullptr || pSrcBlock->Id() == pDestBlock->Id())
			{
				return false;
			}
			_pfn->MergeBlocks(pDestBlock, pSrcBlock);
			_nodeBlocks.Set(src, pDestBlock);
			return true;
		}

		const model::Node* CppCompiler::GetMergeableNode(const model::PortElementBase& elt)
		{
			if (ModelEx::HasSingleDescendant(elt))
			{
				Variable* pVar = GetVariableFor(elt);
				if (pVar != nullptr && !pVar->IsLiteral())
				{
					return ModelEx::GetSourceNode(elt);
				}
			}

			return nullptr;
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
				_nodeBlocks.Clear();
			}
		}

		void CppCompiler::EnsureEmitted(Variable& var)
		{
			if (var.HasEmittedName())
			{
				if (var.IsLiteral() && var.IsScalar())
				{
					Emit(var);
				}
			}
			else
			{
				AllocVar(var);
				if (var.IsNew())
				{
					Emit(var);
				}
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

		Variable* CppCompiler::EnsureEmitted(model::PortElementBase& elt)
		{
			Variable* pVar = EnsureVariableFor(elt);
			EnsureEmitted(pVar);
			return pVar;
		}

		Variable* CppCompiler::EnsureEmitted(model::InputPortBase* pPort)
		{
			assert(pPort != nullptr);
			return EnsureEmitted(pPort->GetInputElement(0));
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

		Variable* CppCompiler::LoadVar(const model::PortElementBase& elt)
		{
			Variable* pVar = EnsureVariableFor(elt);
			EnsureEmitted(pVar);
			_pfn->Value(*pVar, elt.GetIndex());
			return pVar;
		}

		Variable* CppCompiler::LoadVar(model::InputPortBase* pPort)
		{
			assert(pPort != nullptr);
			return LoadVar(pPort->GetInputElement(0));
		}

		const std::string& CppCompiler::LoopVarName()
		{
			return c_LoopVarName;
		}

		void CppCompiler::CompileTypecastNode(const nodes::TypeCastNode<bool, int>& node)
		{
			NewCodeBlock(node);

			// The Cpp compiler currently implements bools using integers. So we don't actually need to do a typecast.
			// We'll just use the already created variable. 

			// Typecast has 1 input and 1 output port
			auto pInput = node.GetInputPorts()[0];
			auto pOutput = node.GetOutputPorts()[0];
			VerifyIsScalar(*pInput);
			VerifyIsScalar(*pOutput);

			Variable* pVar = GetVariableFor(pInput->GetInputElement(0));
			SetVariableFor(pOutput, pVar);
			
			TryMergeCodeBlock(node);
		}
	}
}
