////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRMapCompiler.cpp (model)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRMapCompiler.h"

#include "CompilableNodeUtilities.h"
#include "CompilableNode.h"

// emitters
#include "Variable.h"

namespace ell
{
namespace model
{
    IRMapCompiler::IRMapCompiler()
        : IRMapCompiler("ELL")
    {
    }

    IRMapCompiler::IRMapCompiler(const std::string& moduleName)
        : emitters::IRModuleEmitter(moduleName)
    {
    }

    //
    //
    //
    llvm::Value* IRMapCompiler::EnsureEmitted(PortElementBase& element)
    {
        return EnsureEmitted(*EnsureVariableFor(element));
    }

    llvm::Value* IRMapCompiler::EnsureEmitted(OutputPortBase* pPort)
    {
        auto pVar = GetOrAllocatePortVariable(pPort);
        return EnsureEmitted(*pVar);
    }

    llvm::Value* IRMapCompiler::EnsureEmitted(InputPortBase* pPort)
    {
        assert(pPort != nullptr);
        auto portElement = pPort->GetInputElement(0); // ?
        return EnsureEmitted(portElement);
    }

    void IRMapCompiler::OnBeginCompileNode(const Node& node)
    {
        if (GetCurrentRegion() == nullptr)
        {
            AddRegion(GetCurrentFunction().GetCurrentBlock());
        }
    }

    void IRMapCompiler::OnEndCompileNode(const Node& node)
    {
        assert(GetCurrentRegion() != nullptr);
        auto pCurBlock = GetCurrentFunction().GetCurrentBlock();
        if (pCurBlock != GetCurrentRegion()->End())
        {
            GetCurrentRegion()->SetEnd(pCurBlock);
        }
    }

    const Node* IRMapCompiler::GetUniqueParent(const Node& node)
    {
        auto inputs = node.GetInputPorts();
        const Node* pParentNode = nullptr;
        emitters::IRBlockRegion* pParentRegion = nullptr;
        for (auto input : inputs)
        {
            for (auto parentNode : input->GetParentNodes())
            {
                if (!HasSingleDescendant(*parentNode))
                {
                    return nullptr;
                }
                emitters::IRBlockRegion* pNodeRegion = _nodeBlocks.Get(*parentNode);
                if (pNodeRegion != nullptr)
                {
                    if (pParentRegion != nullptr && pNodeRegion != pParentRegion)
                    {
                        return nullptr;
                    }
                    pParentRegion = pNodeRegion;
                    pParentNode = parentNode;
                }
            }
        }
        return pParentNode;
    }

    void IRMapCompiler::NewBlockRegion(const Node& node)
    {
        auto pBlock = GetCurrentFunction().Block(IdString(node));
        GetCurrentFunction().SetCurrentBlock(pBlock);
        auto currentRegion = AddRegion(pBlock);
        _nodeBlocks.Set(node, currentRegion);
        if (GetCompilerParameters().includeDiagnosticInfo)
        {
            GetCurrentFunction().Print(DiagnosticString(node) + '\n');
        }
    }

    bool IRMapCompiler::TryMergeRegion(const Node& node)
    {
        auto pRegion = _nodeBlocks.Get(node);
        if (pRegion == nullptr)
        {
            return false;
        }

        const Node* pParentNode = GetUniqueParent(node);
        if (pParentNode == nullptr)
        {
            return false;
        }

        return TryMergeRegions(*pParentNode, node);
    }

    bool IRMapCompiler::TryMergeRegions(const Node& dest, const Node& src)
    {
        emitters::IRBlockRegion* pDestRegion = _nodeBlocks.Get(dest);
        if (pDestRegion == nullptr)
        {
            return false;
        }
        return TryMergeNodeIntoRegion(pDestRegion, src);
    }

    bool IRMapCompiler::TryMergeNodeIntoRegion(emitters::IRBlockRegion* pDestRegion, const Node& src)
    {
        emitters::IRBlockRegion* pSrcRegion = _nodeBlocks.Get(src);
        if (pSrcRegion == nullptr || pSrcRegion == pDestRegion)
        {
            return false;
        }
        GetCurrentRegion()->SetEnd(GetCurrentFunction().GetCurrentBlock());
        GetCurrentFunction().ConcatRegions(pDestRegion, pSrcRegion);
        _nodeBlocks.Set(src, pDestRegion);
        return true;
    }

    emitters::IRBlockRegion* IRMapCompiler::GetMergeableRegion(const PortElementBase& element)
    {
        const Node* pNode = nullptr;
        if (HasSingleDescendant(element))
        {
            emitters::Variable* pVar = GetVariableFor(element);
            if (pVar != nullptr && !pVar->IsLiteral())
            {
                pNode = element.ReferencedPort()->GetNode();
            }
        }

        return (pNode != nullptr) ? _nodeBlocks.Get(*pNode) : nullptr;
    }

    llvm::Value* IRMapCompiler::LoadVariable(const PortElementBase& element)
    {
        emitters::Variable* pVar = EnsureVariableFor(element);
        llvm::Value* pVal = EnsureEmitted(*pVar);
        if (pVar->IsScalar())
        {
            if (element.GetIndex() > 0)
            {
                throw emitters::EmitterException(emitters::EmitterError::vectorVariableExpected);
            }
            if (pVar->IsLiteral())
            {
                return pVal;
            }
            return GetCurrentFunction().Load(pVal);
        }

        if (element.GetIndex() >= pVar->Dimension())
        {
            throw emitters::EmitterException(emitters::EmitterError::indexOutOfRange);
        }
        return GetCurrentFunction().ValueAt(pVal, GetCurrentFunction().Literal((int)element.GetIndex()));
    }

    llvm::Value* IRMapCompiler::LoadVariable(InputPortBase* pPort)
    {
        assert(pPort != nullptr);
        return LoadVariable(pPort->GetInputElement(0));
    }
}
}
