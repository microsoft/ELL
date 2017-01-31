////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MakeNodeCompilable.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// emitters
#include "EmitterTypes.h"

// model
#include "ModelTransformer.h"
#include "Node.h"

namespace ell
{
namespace model
{
    //
    // Helper functions for getting info about the model
    //

    /// <summary>True if port has dimension greater than 1, and references exactly one output port</summary>
    bool IsPureVector(const model::InputPortBase& port);

    /// <summary>Does this node have a single descendant?</summary>
    bool HasSingleDescendant(const model::Node& node);

    /// <summary>Does this node have a single descendant?</summary>
    bool HasSingleDescendant(const model::PortElementBase& element);

    /// <summary>Get this node's id as a string</summary>
    std::string IdString(const model::Node& node);

    /// <summary>Get this node's id diagnostic information</summary>
    std::string DiagnosticString(const model::Node& node);

    /// <summary>Convert a PortType to a ValueType</summary>
    emitters::VariableType PortTypeToVariableType(model::Port::PortType type);

    /// <summary>Get this port's value type</summary>
    emitters::VariableType GetPortVariableType(const model::Port& port);

    void VerifyIsScalar(const model::Port& port);
    void VerifyIsPureBinary(const model::Node& node);
}
}
