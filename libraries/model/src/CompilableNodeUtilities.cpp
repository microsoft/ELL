////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MakeNodeCompilable.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableNodeUtilities.h"
#include "EmitterException.h"

// emitters
#include "EmitterTypes.h"

// model
#include "InputNode.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "OutputNode.h"
#include "PortElements.h"

// utiltiies
#include "Exception.h"

// stl
#include <functional>
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace ell
{
namespace model
{
    //
    // Helper functions
    //
    bool IsPureVector(const model::InputPortBase& port)
    {

        const auto& elements = port.GetInputElements();
        return elements.Size() > 1 && elements.IsFullPortOutput();
    }

    bool HasSingleDescendant(const model::Node& node)
    {
        return (node.GetDependentNodes().size() == 1);
    }

    bool HasSingleDescendant(const model::PortElementBase& element)
    {
        return HasSingleDescendant(*(element.ReferencedPort()->GetNode()));
    }

    std::string IdString(const model::Node& node)
    {
        std::stringstream id;
        id << "Node_" << node.GetId();
        return id.str();
    }

    std::string DiagnosticString(const model::Node& node)
    {
        std::stringstream logInfo;
        logInfo << node.GetRuntimeTypeName() << "::" << node.GetId();
        return logInfo.str();
    }

    emitters::VariableType PortTypeToVariableType(model::Port::PortType type)
    {
        switch (type)
        {
            case model::Port::PortType::boolean:
                return emitters::VariableType::Byte;
            case model::Port::PortType::integer:
                return emitters::VariableType::Int32;
            case model::Port::PortType::real:
                return emitters::VariableType::Double;
            default:
                throw emitters::EmitterException(emitters::EmitterError::notSupported, "Port type not supported");
        }
    }

    /// <summary>Get this port's value type</summary>
    emitters::VariableType GetPortVariableType(const model::Port& port)
    {
        return PortTypeToVariableType(port.GetType());
    }

    void VerifyIsScalar(const model::Port& port)
    {
        if (port.Size() != 1)
        {
            throw emitters::EmitterException(emitters::EmitterError::scalarInputsExpected);
        }
    }

    void VerifyIsPureBinary(const model::Node& node)
    {
        if (node.NumInputPorts() != 2)
        {
            // Only support binary right now
            throw emitters::EmitterException(emitters::EmitterError::binaryInputsExpected);
        }
    }
}
}
