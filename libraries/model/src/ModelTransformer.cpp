////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ModelTransformer.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelTransformer.h"
#include "Node.h"

// stl

/// <summary> model namespace </summary>
namespace model
{
    const Port* ModelTransformer::GetCorrespondingPort(const Port* port) { return _portMap[port]; }

    void ModelTransformer::MapPort(const Port* oldPort, const Port* newPort) { _portMap[oldPort] = newPort; }

    Model ModelTransformer::CopyModel()
    {
        _oldModel.Visit([&](const Node& node) { node.Copy(*this); });

        return GetModel();
    }

    Model ModelTransformer::RefineModel()
    {
        _oldModel.Visit([&](const Node& node) { node.Refine(*this); });

        return GetModel();
    }
}
