////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilableNodeUtilities.cpp (model)
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

// utiltiies
#include "Exception.h"
#include "Logger.h"

// stl
#include <sstream>
#include <string>

namespace ell
{
namespace model
{
    using namespace logging;

    //
    // Helper functions
    //
    bool HasSingleDescendant(const Node& node)
    {
        Log() << DiagnosticString(node) << " has " << node.GetDependentNodes().size() << " dependent nodes" << EOL;
        return (node.GetDependentNodes().size() == 1);
    }

    bool HasSingleDescendant(const PortElementBase& element)
    {
        return HasSingleDescendant(*(element.ReferencedPort()->GetNode()));
    }

    std::string IdString(const Node& node)
    {
        return std::string{"Node_"} + to_string(node.GetId());
    }

    std::string DiagnosticString(const Node& node)
    {
        std::stringstream logInfo;
        logInfo << node.GetRuntimeTypeName() << "::" << node.GetId();
        return logInfo.str();
    }

    emitters::VariableType PortTypeToVariableType(Port::PortType type)
    {
        switch (type)
        {
            case Port::PortType::boolean:
                return emitters::VariableType::Byte;
            case Port::PortType::integer:
                return emitters::VariableType::Int32;
            case Port::PortType::bigInt:
                return emitters::VariableType::Int64;
            case Port::PortType::smallReal:
                return emitters::VariableType::Float;
            case Port::PortType::real:
                return emitters::VariableType::Double;
            default:
                throw emitters::EmitterException(emitters::EmitterError::notSupported, "Port type not supported");
        }
    }

    emitters::VariableType GetPortVariableType(const Port& port)
    {
        return PortTypeToVariableType(port.GetType());
    }

    Port::PortType VariableTypeToPortType(emitters::VariableType type)
    {
        switch (type)
        {
            case emitters::VariableType::Byte:
                return Port::PortType::boolean;
            case emitters::VariableType::Int32:
                return Port::PortType::integer;
            case emitters::VariableType::Int64:
                return Port::PortType::bigInt;
            case emitters::VariableType::Float:
                return Port::PortType::smallReal;
            case emitters::VariableType::Double:
                return Port::PortType::real;
            default:
                throw emitters::EmitterException(emitters::EmitterError::notSupported, "Variable type not supported");
        }
    }

    bool IsScalar(const Port& port)
    {
        return port.Size() == 1;
    }

    void VerifyIsScalar(const Port& port)
    {
        if (!IsScalar(port))
        {
            throw emitters::EmitterException(emitters::EmitterError::scalarInputsExpected);
        }
    }

    void VerifyIsPureBinary(const Node& node)
    {
        if (node.NumInputPorts() != 2)
        {
            throw emitters::EmitterException(emitters::EmitterError::binaryInputsExpected);
        }
    }
}
}
