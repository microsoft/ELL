////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     Stack.h (layers)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Layer.h"
#include "CoordinateList.h"

// stl
#include <string>
#include <iostream>
#include <vector>
#include <memory>

namespace layers
{
    /// <summary> Implements a stack of layers. </summary>
    class Stack
    {
    public:
        /// <summary> Adds a layer to the stack. </summary>
        ///
        /// <param name="layer"> The layer to add to the stack. </param>
        ///
        /// <returns> The index of the added layer. </returns>
        uint64_t AddLayer(std::unique_ptr<Layer> layer);

        /// <summary> Returns the number of layers in the stack. </summary>
        ///
        /// <returns> The total number of layers in the stack. </returns>
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
        const Layer& GetLayer(uint64_t layerIndex) const;

        /// <summary> Gets the list of coordinates in a given layer. </summary>
        ///
        /// <param name="layerIndex"> Zero-based index of the layer. </param>
        ///
        /// <returns> The coordinate list. </returns>
        CoordinateList BuildCoordinateList(uint64_t layerIndex) const;

        /// <summary> Saves a stack to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        void Save(std::ostream& os) const; // TODO, move this to common

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName();

        /// <summary> Reads the stack from an XMLDeserializer. </summary>
        ///
        /// <param name="deserializer"> [in,out] The deserializer. </param>
        void Read(utilities::XMLDeserializer& deserializer);

        /// <summary> Writes the stack to an XMLSerializer. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        void Write(utilities::XMLSerializer& serializer) const;

    protected:
        // members
        std::vector<std::unique_ptr<Layer>> _layers;

    private:
        static const int _currentVersion = 1;
    };
}
