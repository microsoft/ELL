////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ModelTransformer.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ModelGraph.h"
#include "Port.h"
#include "InputPort.h"
#include "OutputPort.h"
#include "OutputPortElementList.h"
#include "Node.h"
#include "OutputPort.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <exception>

/// <summary> model namespace </summary>
namespace model
{
    class ModelTransformer
    {
    public:
        ModelTransformer(const Model& model) : _oldModel(model)
        {}

        Model CopyModel();
        Model RefineModel();

        const Port* GetCorrespondingPort(const Port* port);

        // const Node* GetCorrespondingInputNode(const Node* node)
        // {
        //     return nullptr;
        // }

        Model GetModel() { return _model; }

        void MapPort(const Port* oldPort, const Port* newPort);

        template <typename ValueType>
        OutputPortElementList<ValueType> TransformInputPort(const InputPort<ValueType>& input);

        template <typename NodeType, typename... Args>
        std::shared_ptr<NodeType> AddNode(Args&&... args);

    private:
        const Model& _oldModel;
        Model _model;
        std::unordered_map<const Port*, const Port*> _portMap;
    };
}

#include "../tcc/ModelTransformer.tcc"
