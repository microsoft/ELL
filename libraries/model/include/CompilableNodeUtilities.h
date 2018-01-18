////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilableNodeUtilities.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// emitters
#include "EmitterTypes.h"

// model
#include "InputPort.h"
#include "Node.h"
#include "Port.h"
#include "PortElements.h"

// stl
#include <string>

namespace ell
{
namespace model
{
    //
    // Helper functions for getting info about the model
    //

    /// <summary> True if port has dimension greater than 1, and references exactly one output port </summary>
    bool IsPureVector(const InputPortBase& port);

    /// <summary> Indicates if a port is scalar (that is, has a size of 1). </summary>
    bool IsScalar(const Port& port);

    /// <summary> Does this node have a single descendant? </summary>
    bool HasSingleDescendant(const Node& node);

    /// <summary> Does this collection of elements have a single descendant? </summary>
    bool HasSingleDescendant(const PortElementBase& element);

    /// <summary> Get this node's id as a string </summary>
    std::string IdString(const Node& node);

    /// <summary> Get this node's id diagnostic information </summary>
    std::string DiagnosticString(const Node& node);

    /// <summary> Convert a PortType to a VariableType </summary>
    emitters::VariableType PortTypeToVariableType(Port::PortType type);

    /// <summary> Get this port's value type </summary>
    emitters::VariableType GetPortVariableType(const Port& port);

    /// <summary> Convert a VariableType to a ValueType </summary>
    Port::PortType VariableTypeToPortType(emitters::VariableType type);

    /// <summary> Throw an exception if a port isn't scalar </summary>
    void VerifyIsScalar(const Port& port);

    /// <summary> Throw an exception if a node isn't binary (has 2 input ports) </summary>
    void VerifyIsPureBinary(const Node& node);
}
}
