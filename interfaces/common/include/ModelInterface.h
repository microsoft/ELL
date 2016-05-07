////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ModelInterface.h (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Layer.h"
#include "CoordinateList.h"
#include "Model.h"

// stl
#include <string>
#include <iostream>
#include <vector>
#include <memory>

namespace interfaces
{
    /// <summary> Implements a model. </summary>
    class Model
    {
    public:
        /// <summary> Default constructor </summary>
        Model();

        // Model& operator=(Model&&) = default;
        Model(layers::Model&& model);

        /// <summary> Returns the number of layers in the model. </summary>
        ///
        /// <returns> The total number of layers in the model. </returns>
        uint64_t NumLayers() const;

        /// <summary> Returns the minimal required size of a given layer, which is the maximum element associated with that layer plus 1. </summary>
        ///
        /// <param name="layerindex"> The layer index. </param>
        ///
        /// <returns> The required layer size. </returns>
        uint64_t GetRequiredLayerSize(uint64_t layerIndex) const;

        /// <summary> Gets a Layer cast as a specified layer type, used when derived classes add functionality to layers </summary>
        ///
        /// <param name="layerIndex"> Zero-based index of the layer. </param>
        ///
        /// <returns> The requested layer, cast to a const reference of the requested type. </returns>
        const layers::Layer& GetLayer(uint64_t layerIndex) const;

        /// <summary> Gets the list of coordinates in a given layer. </summary>
        ///
        /// <param name="layerIndex"> Zero-based index of the layer. </param>
        ///
        /// <returns> The coordinate list. </returns>
//        layers::CoordinateList BuildCoordinateList(uint64_t layerIndex) const {return _model->BuildCoordinateList(layerIndex);}

        /// <summary> Saves a model to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        void Save(const std::string& inputModelFile) const;

        const layers::Model& GetModel() const;
        layers::Model& GetModel();
        
    private:
        std::shared_ptr<layers::Model> _model;
    };
}
