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

#include <utilities/include/Exception.h>
#include <utilities/include/StringUtil.h>

#include <algorithm>

namespace ell
{
namespace model
{

    // Represents a correspondence between two ports, as when a port is transformed or copied into another model
    struct PortCorrespondence
    {
        const InputPortBase* source;
        const OutputPortBase* destination;
    };

    // Represents a set of correspondences between ports, for use when making copies of submodels onto a new location
    class PortCorrespondences
    {
    public:
        PortCorrespondences() = default;
        PortCorrespondences(const std::vector<const InputPortBase*>& sources, const std::vector<const OutputPortBase*>& destinations);

        void Add(const PortCorrespondence& correspondence);
        PortCorrespondence operator[](int index) const;

        using iterator = std::vector<PortCorrespondence>::iterator;
        using const_iterator = std::vector<PortCorrespondence>::const_iterator;

    private:
        friend PortCorrespondences::iterator begin(PortCorrespondences& correspondences);
        friend PortCorrespondences::iterator end(PortCorrespondences& correspondences);
        friend PortCorrespondences::const_iterator begin(const PortCorrespondences& correspondences);
        friend PortCorrespondences::const_iterator end(const PortCorrespondences& correspondences);

        std::vector<PortCorrespondence> _correspondences;
    };

    //
    // NullNode -- used for deleting nodes
    //
    template <typename ValueType>
    class NullNode : public model::Node
    {
    public:
        const model::OutputPort<ValueType>& output = _output;

        NullNode(size_t size) :
            Node({}, { &_output }),
            _output(this, defaultOutputPortName, size){};
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("NullNode"); }
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Compute() const override{};
        void WriteToArchive(utilities::Archiver& archiver) const override{};
        void ReadFromArchive(utilities::Unarchiver& archiver) override{};

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
    TransformContext::TransformContext() :
        _compiler(nullptr)
    {
    }

    TransformContext::TransformContext(const NodeActionFunction& nodeActionFunction) :
        _compiler(nullptr)
    {
        _nodeActionFunctions.emplace_back(nodeActionFunction);
    }

    TransformContext::TransformContext(const MapCompiler* compiler, const NodeActionFunction& nodeActionFunction) :
        _compiler(compiler)
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
    // PortCorrespondences
    //
    void VerifyOntoCorrespondences(const std::vector<const InputPortBase*>& srcInputs, const std::vector<const OutputPortBase*>& onto);

    PortCorrespondences::PortCorrespondences(const std::vector<const InputPortBase*>& sources, const std::vector<const OutputPortBase*>& destinations)
    {
        VerifyOntoCorrespondences(sources, destinations);
        for (size_t i = 0, end = sources.size(); i < end; ++i)
        {
            _correspondences.push_back({ sources[i], destinations[i] });
        }
    }

    void PortCorrespondences::Add(const PortCorrespondence& correspondence)
    {
        _correspondences.push_back(correspondence);
    }

    PortCorrespondence PortCorrespondences::operator[](int index) const
    {
        return _correspondences[index];
    }

    PortCorrespondences::iterator begin(PortCorrespondences& correspondences)
    {
        return begin(correspondences._correspondences);
    }

    PortCorrespondences::iterator end(PortCorrespondences& correspondences)
    {
        return end(correspondences._correspondences);
    }

    PortCorrespondences::const_iterator begin(const PortCorrespondences& correspondences)
    {
        return begin(correspondences._correspondences);
    }

    PortCorrespondences::const_iterator end(const PortCorrespondences& correspondences)
    {
        return end(correspondences._correspondences);
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

    bool ModelTransformer::PortOutputsMap::IsOutputMapped(const OutputPortBase& queryPort) const
    {
        auto queryPortPtr = &queryPort;
        return (_outputPortMap.find(queryPortPtr) != _outputPortMap.end());
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

    void ModelTransformer::PortOutputsMap::MapNodeOutput(const OutputPortBase* oldPort, const OutputPortBase* newPort)
    {
        if (oldPort->Size() != newPort->Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch,
                                            utilities::FormatString("Trying to map port %s to output of different size, expecting %lld, but found %lld", oldPort->GetName().c_str(), oldPort->Size(), newPort->Size()));
        }
        _outputPortMap[oldPort] = newPort;
    }

    ModelTransformer::PortOutputsMap ModelTransformer::PortOutputsMap::ConcatenateMaps(const PortOutputsMap& prevMap, const PortOutputsMap& newMap)
    {
        PortOutputsMap result;
        for (const auto& entry : prevMap._outputPortMap)
        {
            const auto& newMappedValue = newMap.GetCorrespondingPort(*entry.second);
            result.MapNodeOutput(entry.first, &newMappedValue);
        }

        return result;
    }

    //
    // ModelTransformer implementation
    //
    const OutputPortBase& ModelTransformer::SimplifyOutputs(const PortElementsBase& elements)
    {
        return _model.SimplifyOutputs(elements);
    }

    Model ModelTransformer::CopyModel(const Model& oldModel)
    {
        TransformContext context;
        Submodel m(oldModel, {}, {});
        auto result = CopySubmodel(m, context);
        return std::move(result.GetModel());
    }

    Model ModelTransformer::CopyModel(const Model& oldModel, const TransformContext& context)
    {
        Submodel m(oldModel, {}, {});
        auto result = CopySubmodel(m, context);
        return std::move(result.GetModel());
    }

    Submodel ModelTransformer::CopySubmodel(const Submodel& submodel, const TransformContext& context)
    {
        Model destModel;
        _elementsMap.Clear();
        auto result = TransformSubmodelOnto(submodel, destModel, {}, context, [](const Node& node, ModelTransformer& transformer) {
            transformer.CopyNode(node);
        });

        ResetContext();
        return result;
    }

    Submodel ModelTransformer::CopySubmodelOnto(const Submodel& submodel, Model& destModel, const std::vector<const OutputPortBase*>& onto, const TransformContext& context)
    {
        _elementsMap.Clear();
        auto result = TransformSubmodelOnto(submodel, destModel, {}, context, [](const Node& node, ModelTransformer& transformer) {
            transformer.CopyNode(node);
        });

        ResetContext();
        return result;
    }

    bool ModelTransformer::IsInPlace() const
    {
        return _isInPlace;
    }

    bool ModelTransformer::ShouldCopyNode(const Node& node) const
    {
        if (!IsInPlace())
            return true;

        if (IsInputNode(node))
        {
            return false;
        }

        const auto& inputs = node.GetInputPorts();
        for (auto in : inputs)
        {
            if (IsInputMapped(*in))
            {
                return true;
            }
        }

        return !IsInPlace(); // no inputs, but not an InputNode --- copy if we're in out-of-place mode
    }

    void ModelTransformer::MapNodeOutput(const OutputPortBase& oldPort, const OutputPortBase& newPort)
    {
        _elementsMap.MapNodeOutput(&oldPort, &newPort);
    }

    bool ModelTransformer::IsInputMapped(const InputPortBase& input) const
    {
        return _elementsMap.IsOutputMapped(input.GetReferencedPort());
    }

    bool ModelTransformer::IsOutputMapped(const OutputPortBase& output) const
    {
        return _elementsMap.IsOutputMapped(output);
    }

    bool ModelTransformer::IsInputNode(const Node& node) const
    {
        return dynamic_cast<const InputNodeBase*>(&node) != nullptr;
    }

    Model ModelTransformer::RefineModel(const Model& oldModel, const TransformContext& context, int maxIterations)
    {
        if (maxIterations <= 0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "maxIterations must be positive");
        }

        _elementsMap.Clear();
        _model = CopyModel(oldModel, context);
        _context = context;

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
            currentModel.Visit([this, &didRefineAny](const Node& node) {
                bool didRefineNode = RefineNode(node);
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

        ResetContext();
        return std::move(_model);
    }

    bool ModelTransformer::Compatible(const InputPortBase* source, const OutputPortBase* dest)
    {
        return (source->Size() == dest->Size()) && (source->GetType() == dest->GetType());
    }

    void ModelTransformer::MapCorrespondingInputs(const std::vector<const InputPortBase*>& sources, const std::vector<const OutputPortBase*>& destinations)
    {
        // Set up port mapping between the given source inputs and their corresponding destination inputs
        // throw an exception if the inputs are ill-formed
        PortCorrespondences correspondences(sources, destinations);
        for (auto correspondence : correspondences)
        {
            auto source = correspondence.source;
            auto dest = correspondence.destination;
            if (!Compatible(source, dest))
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible source and destination inputs");
            }
            MapNodeOutput(source->GetReferencedPort(), *dest);
        }
    }

    Model ModelTransformer::TransformModel(const Model& model, const TransformContext& context, const NodeTransformFunction& transformFunction)
    {
        Model newModel;
        Submodel submodel(model, {}, {});
        auto result = TransformSubmodelOnto(submodel, newModel, {}, context, transformFunction);
        return result.GetModel().ShallowCopy();
    }

    Submodel ModelTransformer::TransformSubmodelOnto(const Submodel& submodel, Model& destModel, const std::vector<const OutputPortBase*>& onto, const TransformContext& context, const NodeTransformFunction& transformFunction)
    {
        _context = context;
        _model = destModel.ShallowCopy();
        _isInPlace = (submodel.GetModel() == destModel);
        auto previousElementMap = std::move(_elementsMap);
        _elementsMap.Clear();

        VerifyOntoCorrespondences(submodel.GetInputPorts(), onto);

        std::vector<const InputPortBase*> sourceInputs = submodel.GetInputPorts();
        MapCorrespondingInputs(sourceInputs, onto);
        submodel.GetModel().VisitSubmodel(sourceInputs, submodel.GetOutputPorts(), [this, transformFunction](const Node& node) {
            transformFunction(node, *this);
            AssignNodeAncestor(node);
        });

        if (!previousElementMap.IsEmpty())
        {
            // Now we have 2 maps, the previous one mapping A->B, and a new one mapping B->C (in _elementsMap).
            // Concatenate them to get a map A->C, and keep it.
            auto newElementsMap = PortOutputsMap::ConcatenateMaps(previousElementMap, _elementsMap);
            _elementsMap = newElementsMap;
        }
        ResetContext();

        _model = Model();

        auto newOutputs = GetCorrespondingOutputs(submodel.GetOutputPorts());

        // Now visit the new outputs until we encounter an input port referencing a port from the "onto" list. That will be one of the new inputs. We should encounter one for each of the "onto" items.
        std::vector<const InputPortBase*> newInputs(submodel.GetInputPorts().size(), nullptr);
        std::unordered_map<const OutputPortBase*, int> ontoPortToIndexMap;
        for (int i = 0, size = (int)onto.size(); i < size; ++i)
        {
            ontoPortToIndexMap[onto[i]] = i;
        }
        destModel.VisitSubmodel(newOutputs, [&ontoPortToIndexMap, &newInputs](const Node& node) {
            // check node's input ports -- if one of them references one of the onto ports, set it in the newInputs vector at the corresponding location
            for (auto input : node.GetInputPorts())
            {
                auto referencedPort = &input->GetReferencedPort();
                if (ontoPortToIndexMap.find(referencedPort) != ontoPortToIndexMap.end())
                {
                    auto index = ontoPortToIndexMap[referencedPort];
                    newInputs[index] = input;
                    ontoPortToIndexMap.erase(referencedPort);
                }
            }
        });

        return { destModel.ShallowCopy(), newInputs, newOutputs };
    }

    void VerifyOntoCorrespondences(const std::vector<const InputPortBase*>& sources, const std::vector<const OutputPortBase*>& destinations)
    {
        if (sources.size() != destinations.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Trying to graft a submodel onto a destination with a different number of outputs");
        }

        bool ok = std::equal(sources.begin(), sources.end(), destinations.begin(), [](const InputPortBase* s, const OutputPortBase* d) {
            return s->GetType() == d->GetType();
        });

        if (!ok)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Trying to graft a submodel onto a destination with an incompatible port type");
        }
    }

    Submodel ModelTransformer::TransformSubmodelOnto(const Submodel& submodel, const std::vector<const OutputPortBase*>& onto, const TransformContext& context, const NodeTransformFunction& transformFunction)
    {
        auto destModel = submodel.GetModel().ShallowCopy();
        return TransformSubmodelOnto(submodel, destModel, onto, context, transformFunction);
    }

    void ModelTransformer::Reset()
    {
        ResetContext();
        _model = Model();
        _elementsMap.Clear();
        _isModelCompilable = false;
    }

    void ModelTransformer::ResetContext()
    {
        _context = TransformContext();
    }

    const OutputPortBase& ModelTransformer::GetCorrespondingInputs(const InputPortBase& port) const
    {
        return GetCorrespondingOutputs(port);
    }

    const OutputPortBase& ModelTransformer::GetCorrespondingOutputs(const InputPortBase& port) const
    {
        return GetCorrespondingOutputs(port.GetReferencedPort());
    }

    const OutputPortBase& ModelTransformer::GetCorrespondingOutputs(const OutputPortBase& port) const
    {
        if (IsInPlace() && !IsOutputMapped(port))
        {
            return port;
        }
        return _elementsMap.GetCorrespondingPort(port);
    }

    std::vector<const OutputPortBase*> ModelTransformer::GetCorrespondingOutputs(const std::vector<const OutputPortBase*>& ports) const
    {
        std::vector<const OutputPortBase*> result;
        for (auto output : ports)
        {
            result.push_back(&GetCorrespondingOutputs(*output));
        }
        return result;
    }

    const OutputPortBase& ModelTransformer::GetCorrespondingOutputs(const PortElementsBase& elements) const
    {
        return GetCorrespondingOutputs(*elements.GetRanges()[0].ReferencedPort());
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
            auto layout = outputPort->GetMemoryLayout().GetExtent();
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
        if (ShouldCopyNode(node))
        {
            node.Copy(*this);

            if (IsOutputMapped(*node.GetOutputPort(0)))
            {
                const auto& port = GetCorrespondingOutputs(*node.GetOutputPort(0));
                auto newNode = const_cast<Node*>(port.GetNode());
                if (newNode && newNode != (&node))
                {
                    // copy metadata
                    newNode->GetMetadata() = node.GetMetadata();
                }
            }
        }
    }

    bool ModelTransformer::RefineNode(const Node& node)
    {
        // If the node action is "refine" or the default, try to refine the node, otherwise leave it alone
        auto action = GetContext().GetNodeAction(node);
        if (action == NodeAction::refine || action == NodeAction::abstain)
        {
            auto didRefineNode = node.InvokeRefine(*this);
            AssignNodeAncestor(node);
            return didRefineNode;
        }
        else
        {
            CopyNode(node);
        }

        return false;
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

    void ModelTransformer::AssignNodeAncestor(const Node& ancestorNode)
    {
        auto iter = _model.GetReverseNodeIterator();
        while (iter.IsValid())
        {
            auto node = const_cast<Node*>(iter.Get());
            if (node->GetMetadata().HasEntry("ancestor"))
            {
                break;
            }
            else
            {
                if (ancestorNode.GetMetadata().HasEntry("ancestor"))
                {
                    node->GetMetadata().SetEntry("ancestor", ancestorNode.GetMetadata().GetEntry<std::string>("ancestor"));
                }
                else
                {
                    node->GetMetadata().SetEntry("ancestor", ancestorNode.GetId().ToString());
                }
            }
            iter.Next();
        }
    }

} // namespace model
} // namespace ell
