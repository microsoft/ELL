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
#include "OutputNode.h"

// utilities
#include "Exception.h"
#include "StringUtil.h"

namespace ell
{
namespace model
{
    //
    // NullNode -- used for deleting nodes
    //
    template <typename ValueType>
    class NullNode : public model::Node
    {
    public:
        const model::OutputPort<ValueType>& output = _output;

        NullNode(size_t size) : Node({}, { &_output }), _output(this, defaultOutputPortName, size) {};
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("NullNode"); }
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Compute() const override {};
        void WriteToArchive(utilities::Archiver& archiver) const override {};
        void ReadFromArchive(utilities::Unarchiver& archiver) override {};

    private:
        void Copy(model::ModelTransformer& transformer) const override
        {
            auto newNode = transformer.AddNode<NullNode<ValueType>>(_output.Size());
            transformer.MapNodeOutput(output, newNode->output);
        }

        model::OutputPort<ValueType> _output;
    };

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
    void ModelTransformer::PortOutputsMap::Clear()
    {
        _outputPortMap.clear();
    }

    bool ModelTransformer::PortOutputsMap::IsEmpty() const
    {
        return _outputPortMap.empty();
    }

    const OutputPortBase& ModelTransformer::PortOutputsMap::GetCorrespondingPort(const OutputPortBase& queryPort) const
    {
        using namespace std::string_literals;
        auto queryPortPtr = &queryPort;
        if (_outputPortMap.find(queryPortPtr) == _outputPortMap.end())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Could not find element "s + to_string(queryPort.GetNode()->GetId()) + "." + queryPort.GetName() + " in new model.");
        }

        auto targetPort = _outputPortMap.at(queryPortPtr);

        if (targetPort->Size() != queryPort.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch,
                utilities::FormatString("Model transformation resulted in a mismatching port size, expecting %lld, but found %lld", queryPort.Size(), targetPort->Size()));
        }
        return *targetPort;
    }

    void ModelTransformer::PortOutputsMap::MapNodeOutput(const OutputPortBase* oldPort, const OutputPortBase& newPort)
    {
        if (oldPort->Size() != newPort.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch,
                utilities::FormatString("Trying to map port %s to output of different size, expecting %lld, but found %lld", oldPort->GetName().c_str(), oldPort->Size(), newPort.Size()));
        }
        _outputPortMap[oldPort] = &newPort;
    }

    ModelTransformer::PortOutputsMap ModelTransformer::PortOutputsMap::ConcatenateMaps(const PortOutputsMap& prevMap, const PortOutputsMap& newMap)
    {
        PortOutputsMap result;
        for (const auto& entry : prevMap._outputPortMap)
        {
            const auto& newMappedValue = newMap.GetCorrespondingPort(*entry.second);
            result.MapNodeOutput(entry.first, newMappedValue);
        }

        return result;
    }

    //
    // ModelTransformer implementation
    //
    Model ModelTransformer::CopyModel(const Model& oldModel, const TransformContext& context)
    {
        std::vector<const Node*> nothing;
        return CopyModel(oldModel, nothing, context);
    }

    Model ModelTransformer::CopyModel(const Model& oldModel, const Node* outputNode, const TransformContext& context)
    {
        return CopyModel(oldModel, std::vector<const Node*>{outputNode}, context);
    }

    Model ModelTransformer::CopyModel(const Model& oldModel, const std::vector<const Node*>& outputNodes, const TransformContext& context)
    {
        _context = context;
        _model = Model();
        _elementsMap.Clear();
        oldModel.VisitSubset(outputNodes, [this](const Node& node) {
            CopyNode(node); });
        _context = TransformContext();
        
        // Copy all the node metadata
        oldModel.VisitSubset(outputNodes, [this](const Node& node) {
            if (node.NumOutputPorts() > 0)
            {
                const auto& port = _elementsMap.GetCorrespondingPort(*node.GetOutputPort(0));
                auto newNode = const_cast<Node*>(port.GetNode());
                if (newNode)
                {
                    newNode->GetMetadata() = node.GetMetadata();
                }
            }
        });

        return std::move(_model);
    }

    Model ModelTransformer::RefineModel(const Model& oldModel, const TransformContext& context, int maxIterations)
    {
        if (maxIterations <= 0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "maxIterations must be positive");
        }

        _context = context;
        _elementsMap.Clear();
        _model = CopyModel(oldModel, context);
        
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
                    CopyNode(node);
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
        _context = TransformContext(); // reset context
        _elementsMap.Clear();
        _isModelCompilable = false;
    }

    const OutputPortBase& ModelTransformer::GetCorrespondingInputs(const InputPortBase& port) const
    {
        return _elementsMap.GetCorrespondingPort(port.GetReferencedPort());
    }

    const OutputPortBase& ModelTransformer::GetCorrespondingOutputs(const OutputPortBase& port) const
    {
        return _elementsMap.GetCorrespondingPort(port);
    }

    const OutputPortBase& ModelTransformer::GetCorrespondingOutputs(const PortElementsBase& elements) const
    {
        return _elementsMap.GetCorrespondingPort(*elements.GetRanges()[0].ReferencedPort());
    }

    InputNodeBase* ModelTransformer::GetCorrespondingInputNode(const InputNodeBase* inputNode) const
    {
        return GetCorrespondingInputNodeAs(inputNode);
    }

    void ModelTransformer::DeleteNode(const Node& node)
    {
        using PortType = Port::PortType;

        const auto& outputPorts = node.GetOutputPorts();
        for (auto outputPort : outputPorts)
        {
            auto layout = outputPort->GetMemoryLayout().GetStride();
            switch (outputPort->GetType())
            {
            case PortType::boolean:
            {
                auto outputNode = AddNode<NullNode<bool>>(outputPort->Size());
                MapNodeOutput(*static_cast<const OutputPort<bool>*>(outputPort), outputNode->output);
                break;
            }
            case PortType::integer:
            {
                auto outputNode = AddNode<NullNode<int>>(outputPort->Size());
                MapNodeOutput(*static_cast<const OutputPort<int>*>(outputPort), outputNode->output);
                break;
            }
            case PortType::bigInt:
            {
                auto outputNode = AddNode<NullNode<std::int64_t>>(outputPort->Size());
                MapNodeOutput(*static_cast<const OutputPort<std::int64_t>*>(outputPort), outputNode->output);
                break;
            }
            case PortType::smallReal:
            {
                auto outputNode = AddNode<NullNode<float>>(outputPort->Size());
                MapNodeOutput(*static_cast<const OutputPort<float>*>(outputPort), outputNode->output);
                break;
            }
            case PortType::real:
            {
                auto outputNode = AddNode<NullNode<double>>(outputPort->Size());
                MapNodeOutput(*static_cast<const OutputPort<double>*>(outputPort), outputNode->output);
                break;
            }
            default:
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Unknown port type");
            }
        }
    }

    void ModelTransformer::CopyNode(const Node& node)
    {
        return node.Copy(*this);
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
