////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelTransformer.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelTransformer.h"
#include "InputNode.h"
#include "Node.h"

// utilities
#include "Exception.h"

namespace ell
{
namespace model
{
    //
    // TransformContext implementation
    //
    TransformContext::TransformContext()
        : _compiler(nullptr)
    {
    }

    TransformContext::TransformContext(const NodeActionFunction& nodeActionFunction)
        : _compiler(nullptr)
    {
        _nodeActionFunctions.emplace_back(nodeActionFunction);
    }

    TransformContext::TransformContext(const MapCompiler* compiler, const NodeActionFunction& nodeActionFunction)
        : _compiler(compiler)
    {
        _nodeActionFunctions.emplace_back(nodeActionFunction);
    }

    bool TransformContext::IsNodeCompilable(const Node& node) const
    {
        return node.IsCompilable(_compiler);
    }

    void TransformContext::AddNodeActionFunction(const NodeActionFunction& nodeActionFunction)
    {
        _nodeActionFunctions.emplace_back(nodeActionFunction);
    }

    NodeAction TransformContext::GetNodeAction(const Node& node) const
    {
        for (auto iter = _nodeActionFunctions.rbegin(); iter != _nodeActionFunctions.rend(); ++iter)
        {
            auto& actionFunction = *iter;
            auto action = actionFunction(node);
            if (action != NodeAction::abstain)
            {
                return action;
            }
        }
        return node.IsCompilable(_compiler) ? NodeAction::compile : NodeAction::refine;
    }

    //
    // PortOutputsMap
    //
    void PortOutputsMap::Clear()
    {
        _map.clear();
    }

    bool PortOutputsMap::IsEmpty() const
    {
        return _map.size() == 0;
    }

    PortElementsBase PortOutputsMap::GetCorrespondingPortElements(const PortElementsBase& queryElements) const
    {
        using namespace std::string_literals;
        PortElementsBase result;
        auto&& queryRanges = queryElements.GetRanges();
        for (auto&& queryRange : queryRanges)
        {
            auto queryRangePort = queryRange.ReferencedPort();
            assert(queryRangePort != nullptr);
            auto queryRangeStartIndex = queryRange.GetStartIndex();
            auto queryRangeSize = queryRange.Size();
            auto queryRangeEnd = queryRangeStartIndex + queryRangeSize;

            // get elements for port
            if (_map.find(queryRangePort) == _map.end())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Could not find element "s + to_string(queryRangePort->GetNode()->GetId()) + "." + queryRangePort->GetName() + " in new model.");
            }

            PortElementsBase portElements = _map.at(queryRangePort);
            size_t targetRangeOffset = 0;
            auto&& targetRanges = portElements.GetRanges();
            for (auto&& targetRange : targetRanges)
            {
                auto targetRangePort = targetRange.ReferencedPort();
                auto targetRangeSize = targetRange.Size();
                auto targetRangeEnd = targetRangeOffset + targetRangeSize;

                bool rangesIntersect = queryRangeEnd > targetRangeOffset && queryRangeStartIndex < targetRangeEnd;
                if (rangesIntersect)
                {
                    // Get relevant subset of targetRange and append it to result;
                    auto maxBegin = std::max(queryRangeStartIndex, targetRangeOffset);
                    auto minEnd = std::min(queryRangeEnd, targetRangeEnd);
                    assert(minEnd >= maxBegin);
                    auto intersectionSize = minEnd - maxBegin;
                    queryRangeStartIndex += intersectionSize;
                    assert(queryRangeSize >= intersectionSize);
                    queryRangeSize -= intersectionSize;
                    result.Append(PortRange(*targetRangePort, targetRange.GetStartIndex(), intersectionSize));

                    // If we've matched all the elements of the query range, we can break out of this loop
                    if (queryRangeSize == 0)
                        break;
                }

                targetRangeOffset += targetRangeSize;
            }
        }

        result.Consolidate();
        assert(result.Size() == queryElements.Size());
        return result;
    }

    void PortOutputsMap::MapNodeOutput(const OutputPortBase* oldPort, const PortElementsBase& newElements)
    {
        _map[oldPort] = newElements;
    }

    PortOutputsMap PortOutputsMap::ConcatenateMaps(const PortOutputsMap& prevMap, const PortOutputsMap& newMap)
    {
        PortOutputsMap result;
        for (const auto& entry : prevMap._map)
        {
            auto newMappedValue = newMap.GetCorrespondingPortElements(entry.second);
            result.MapNodeOutput(entry.first, newMappedValue);
        }
        return result;
    }

    //
    // ModelTransformer implementation
    //
    Model ModelTransformer::CopyModel(const Model& oldModel, const TransformContext& context)
    {
        _context = context;
        _model = Model();
        _elementsMap.Clear();
        oldModel.Visit([this](const Node& node) { node.InvokeCopy(*this); });
        _context = TransformContext();

        return std::move(_model);
    }

    Model ModelTransformer::CopyModel(const Model& oldModel, const Node* outputNode, const TransformContext& context)
    {
        _context = context;
        _model = Model();
        _elementsMap.Clear();
        oldModel.VisitSubset(outputNode, [this](const Node& node) { node.InvokeCopy(*this); });
        _context = TransformContext();

        return std::move(_model);
    }

    Model ModelTransformer::CopyModel(const Model& oldModel, const std::vector<const Node*>& outputNodes, const TransformContext& context)
    {
        _context = context;
        _model = Model();
        _elementsMap.Clear();
        oldModel.VisitSubset(outputNodes, [this](const Node& node) { node.InvokeCopy(*this); });
        _context = TransformContext();

        return std::move(_model);
    }

    Model ModelTransformer::RefineModel(Model oldModel, const TransformContext& context, int maxIterations)
    {
        if (maxIterations <= 0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "maxIterations must be positive");
        }

        _context = context;
        _model = std::move(oldModel);
        _elementsMap.Clear();

        // Refine until all nodes are compilable according to context.IsNodeCompilable(), until
        // the model is fully refined, or until the maximum number of iterations is reached.
        for (int i = 0; i < maxIterations; ++i)
        {
            Model currentModel = std::move(_model);
            _model = Model();

            auto previousElementMap = std::move(_elementsMap);
            _elementsMap.Clear();

            _isModelCompilable = true;

            // Do one refinement pass
            // Note: as a side-effect, _elementsMap may be modified
            bool didRefineAny = false;
            currentModel.Visit([this, &context, &didRefineAny](const Node& node) {
                bool didRefineNode = false;
                auto action = context.GetNodeAction(node);
                // If the node action is "refine" or the default, try to refine the node, otherwise leave it alone
                if (action == NodeAction::refine || action == NodeAction::abstain)
                {
                    didRefineNode = node.InvokeRefine(*this);
                }
                else
                {
                    node.InvokeCopy(*this);
                }
                didRefineAny |= didRefineNode;
            });

            if (!previousElementMap.IsEmpty())
            {
                // Now we have 2 maps, the previous one mapping A->B, and a new one mapping B->C (in _elementsMap).
                // Concatenate them to get a map A->C, and keep it.
                auto newElementsMap = PortOutputsMap::ConcatenateMaps(previousElementMap, _elementsMap);
                _elementsMap = newElementsMap;
            }

            // check for early end condition
            if (!didRefineAny || _isModelCompilable)
            {
                break;
            }
        }

        // clear out the context
        _context = TransformContext();
        return std::move(_model);
    }

    Model ModelTransformer::TransformModel(const Model& oldModel, const TransformContext& context, const std::function<void(const Node&, ModelTransformer&)>& transformFunction)
    {
        _context = context;
        _model = Model();
        auto previousElementMap = std::move(_elementsMap);
        _elementsMap.Clear();

        oldModel.Visit([this, transformFunction](const Node& node) { transformFunction(node, *this); });

        if (!previousElementMap.IsEmpty())
        {
            // Now we have 2 maps, the previous one mapping A->B, and a new one mapping B->C (in _elementsMap).
            // Concatenate them to get a map A->C, and keep it.
            auto newElementsMap = PortOutputsMap::ConcatenateMaps(previousElementMap, _elementsMap);
            _elementsMap = newElementsMap;
        }
        _context = TransformContext(); // reset context
        return std::move(_model);
    }

    void ModelTransformer::Reset()
    {
        _elementsMap.Clear();        
    }

    PortElementsBase ModelTransformer::TransformPortElements(const PortElementsBase& elements) const
    {
        return _elementsMap.GetCorrespondingPortElements(elements);
    }

    PortElementsBase ModelTransformer::GetCorrespondingOutputs(const OutputPortBase& port) const
    {
        return _elementsMap.GetCorrespondingPortElements(PortElementsBase(port));
    }

    PortElementsBase ModelTransformer::GetCorrespondingOutputs(const PortElementsBase& elements) const
    {
        return _elementsMap.GetCorrespondingPortElements(elements);
    }

    InputNodeBase* ModelTransformer::GetCorrespondingInputNode(const InputNodeBase* inputNode) const
    {
        return GetCorrespondingInputNodeAs(inputNode);
    }

    std::vector<const Node*> ModelTransformer::FindUncompilableNodes(const Model& model, const TransformContext& context) const
    {
        std::vector<const Node*> uncompilableNodes;

        auto iter = model.GetNodeIterator();
        while (iter.IsValid())
        {
            auto node = iter.Get();
            if (!context.IsNodeCompilable(*node))
            {
                uncompilableNodes.push_back(node);
            }
            iter.Next();
        }
        return uncompilableNodes;
    }
}
}
