////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Node.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Node.h"
#include "InputPort.h"
#include "Model.h"
#include "ModelTransformer.h"
#include "OutputPort.h"

// utilities
#include "IArchivable.h"

// stl
#include <unordered_set>

namespace ell
{
namespace model
{
    namespace
    {
        //
        // Relevant archive format versions
        //
        constexpr utilities::ArchiveVersion noMetadataArchiveVersion = {utilities::ArchiveVersionNumbers::v0_initial};
        constexpr utilities::ArchiveVersion metadataArchiveVersion = {utilities::ArchiveVersionNumbers::v3_model_metadata};
    }

    Node::Node(const std::vector<InputPortBase*>& inputs, const std::vector<OutputPortBase*>& outputs)
        : _id(NodeId()), _inputs(inputs), _outputs(outputs){};

    void Node::AddInputPort(InputPortBase* input)
    {
        _inputs.push_back(input);
    }

    void Node::AddOutputPort(OutputPortBase* output)
    {
        _outputs.push_back(output);
    }

    InputPortBase* Node::GetInputPort(const std::string& portName)
    {
        for (auto port : _inputs)
        {
            if (port->GetName() == portName)
            {
                return port;
            }
        }
        return nullptr;
    }

    const InputPortBase* Node::GetInputPort(const std::string& portName) const
    {
        for (auto port : _inputs)
        {
            if (port->GetName() == portName)
            {
                return port;
            }
        }
        return nullptr;
    }

    InputPortBase* Node::GetInputPort(size_t portIndex)
    {
        return _inputs[portIndex];
    }

    const InputPortBase* Node::GetInputPort(size_t portIndex) const
    {
        return _inputs[portIndex];
    }

    OutputPortBase* Node::GetOutputPort(const std::string& portName)
    {
        for (auto port : _outputs)
        {
            if (port->GetName() == portName)
            {
                return port;
            }
        }
        return nullptr;
    }

    const OutputPortBase* Node::GetOutputPort(const std::string& portName) const
    {
        for (auto port : _outputs)
        {
            if (port->GetName() == portName)
            {
                return port;
            }
        }
        return nullptr;
    }

    OutputPortBase* Node::GetOutputPort(size_t portIndex)
    {
        return _outputs[portIndex];
    }

    const OutputPortBase* Node::GetOutputPort(size_t portIndex) const
    {
        return _outputs[portIndex];
    }

    Port* Node::GetPort(const std::string& portName)
    {
        auto inputPort = GetInputPort(portName);
        if (inputPort != nullptr)
        {
            return inputPort;
        }

        return GetOutputPort(portName);
    }

    const Port* Node::GetPort(const std::string& portName) const
    {
        auto inputPort = GetInputPort(portName);
        if (inputPort != nullptr)
        {
            return inputPort;
        }

        return GetOutputPort(portName);
    }

    std::vector<const Node*> Node::GetParentNodes() const
    {
        std::unordered_set<const Node*> nodes;
        for (const auto& port : _inputs)
        {
            for (const auto& node : port->GetParentNodes())
            {
                nodes.insert(node);
            }
        }
        return std::vector<const Node*>{ nodes.begin(), nodes.end() };
    }

    void Node::AddDependent(const Node* dependent) const
    {
        _dependentNodes.push_back(dependent);
    }

    void Node::RegisterDependencies() const
    {
        for (const auto& input : _inputs)
        {
            for (const auto& node : input->GetParentNodes())
            {
                node->AddDependent(this);
            }
            for (const auto& range : input->GetInputElements().GetRanges())
            {
                range.ReferencedPort()->ReferencePort();
            }
        }
    }

    void Node::InvokeCopy(ModelTransformer& transformer) const
    {
        Copy(transformer);
    }

    bool Node::InvokeRefine(ModelTransformer& transformer) const
    {
        return Refine(transformer);
    }

    // Default implementation of Refine just copies and returns false
    bool Node::Refine(ModelTransformer& transformer) const
    {
        Copy(transformer);
        return false;
    }

    void Node::Print(std::ostream& os) const
    {
        bool isFirstInputPort = true;
        os << "node_" << GetId() << " (" << std::hex << this << std::dec << ") = " << GetRuntimeTypeName() << "(";
        for (const auto& inputPort : GetInputPorts())
        {
            os << (isFirstInputPort ? "" : ", ");
            isFirstInputPort = false;

            auto elements = inputPort->GetInputElements();
            if (elements.NumRanges() > 1)
            {
                os << "{";
            }

            bool isFirstRange = true;
            for (const auto& range : elements.GetRanges())
            {
                os << (isFirstRange ? "" : ", ");
                isFirstRange = false;

                auto port = range.ReferencedPort();
                os << "node_" << port->GetNode()->GetId() << "." << port->GetName();
                if (!range.IsFullPortRange())
                {
                    auto start = range.GetStartIndex();
                    auto size = range.Size();
                    os << "[" << start << ":" << (start + size) << "]";
                }
            }

            if (elements.NumRanges() > 1)
            {
                os << "}";
            }
        }
        os << ")" << std::endl;
    }

    utilities::ArchiveVersion Node::GetArchiveVersion() const
    {
        if(_metadata.IsEmpty())
        {
            return noMetadataArchiveVersion;
        }
        else
        {
            return metadataArchiveVersion;
        }
    }

    bool Node::CanReadArchiveVersion(const utilities::ArchiveVersion& version) const
    {
        return version >= noMetadataArchiveVersion && version <= metadataArchiveVersion;
    }

    void Node::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["id"] << _id;
        if(!_metadata.IsEmpty())
        {
            archiver["metadata"] << _metadata;
        }
    }

    void Node::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        NodeId oldId;        
        archiver["id"] >> oldId;
        _id = oldId;
        archiver.OptionalProperty("metadata") >> _metadata;

        auto& context = archiver.GetContext();
        ModelSerializationContext& newContext = dynamic_cast<ModelSerializationContext&>(context);
        newContext.MapNode(oldId, this);
    }
}
}
