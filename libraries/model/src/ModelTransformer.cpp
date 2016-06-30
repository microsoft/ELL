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
    const Port* ModelTransformer::GetCorrespondingPort(const Port& port) { return _portMap[&port]; }

    void ModelTransformer::MapPort(const Port& oldPort, const Port& newPort)
    {
        // this is hideous
        auto nonconstPort = const_cast<Port*>(&newPort);
        _portMap[&oldPort] = nonconstPort;
    }

    Model ModelTransformer::CopyModel(const Model& oldModel)
    {
        _model = Model();
        _portMap.clear();
        oldModel.Visit([&](const Node& node) { node.Copy(*this); });

        return _model;
    }

    Model ModelTransformer::RefineModel(const Model& oldModel)
    {
        _model = Model();
        _portMap.clear();
        oldModel.Visit([&](const Node& node) { node.Refine(*this); });

        return _model;
    }
}
