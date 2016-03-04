////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Map.h (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Layer.h"
#include "Coordinate.h"

// types
#include "types.h"
#include "RealArray.h"

// stl
#include <vector>
#include <memory>
#include <type_traits>

namespace layers
{
    /// <summary> Implements a map. </summary>
    class Map
    {
    public:

        /// <summary> An iterator over the output values of the map. </summary>
        class Iterator : public IIndexValueIterator
        {
        public:

            /// <summary> Copy constructor. </summary>
            Iterator(const Iterator&) = default;

            /// <summary> Move constructor. </summary>
            Iterator(Iterator&&) = default;

            /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
            ///
            /// <returns> true if valid, false if not. </returns>
            bool IsValid() const;

            /// <summary> Proceeds to the Next iterate. </summary>
            void Next();

            /// <summary> Gets the current index-value pair. </summary>
            ///
            /// <returns> The current index-value pair. </returns>
            IndexValue Get() const;

        protected:
            std::shared_ptr<std::vector<types::DoubleArray>> _spOutputs;
            const CoordinateList _outputCoordinates;
            uint64 _index;

            // private ctor, can only be called from Map class
            Iterator(std::shared_ptr<std::vector<types::DoubleArray>> spOutput, const CoordinateList& outputCoordinates);
            friend Map;
        };

        /// <summary> Default constructor. </summary>
        Map() = default;

        /// <summary> Constructs an instance of Map. </summary>
        ///
        /// <param name="inputLayerSize"> Input dimension. </param>
        Map(uint64 inputLayerSize);

        /// <summary> Virtual destructor. </summary>
        virtual ~Map() = default;

        /// <summary> Computes the Map. </summary>
        ///
        /// <typeparam name="IndexValueIteratorType"> Input iterator type. </typeparam>
        /// <param name="IndexValueIterator"> The input value iterator. </param>
        /// <param name="outputCoordinates"> The output coordinates. </param>
        ///
        /// <returns> An Iterator over output values. </returns>
        template <typename IndexValueIteratorType, typename concept = std::enable_if_t<std::is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        Iterator Compute(IndexValueIteratorType IndexValueIterator, const CoordinateList& outputCoordinates) const;

        /// <summary> Adds a layer to the map. </summary>
        ///
        /// <param name="layer"> The layer to add to the map. </param>
        ///
        /// <returns> The index of the added layer. </returns>
        uint64 PushBack(std::shared_ptr<Layer> layer);

        /// <summary> Returns the number of layers in the map. </summary>
        ///
        /// <returns> The total number of layers in the map. </returns>
        uint64 NumLayers() const;

        /// <summary> Gets a Layer cast as a specified layer type, used when derived classes add functionality to layers </summary>
        ///
        /// <typeparam name="LayerType"> Layer type to return. </typeparam>
        /// <param name="layerIndex"> Zero-based index of the layer. </param>
        ///
        /// <returns> The requested layer, cast to the requested type. </returns>
        template<typename LayerType = Layer>
        std::shared_ptr<const LayerType> GetLayer(uint64 layerIndex) const;

        /// <summary> Static function that loads a Map from file. </summary>
        ///
        /// <typeparam name="MapType"> Map type to load. </typeparam>
        /// <param name="inputMapFile"> Name of the map file to load. </param>
        ///
        /// <returns> A MapType. </returns>
        template<typename MapType = Map>
        static MapType Load(const std::string& inputMapFile);

        /// <summary> Serializes the Map in json format. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        void Serialize(utilities::JsonSerializer& serializer) const;

        /// <summary> Serializes the Map in json format. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        void Serialize(ostream& os) const;

        /// <summary> Deserializes the Map in json format. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        virtual void Deserialize(utilities::JsonSerializer& serializer);

        /// <summary> Static function for deserializing a std::shared_ptr that points to a Layer </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        /// <param name="up"> [in,out] The pointer to the layer. </param>
        static void DeserializeLayers(utilities::JsonSerializer& serializer, std::shared_ptr<Layer>& spLayer);

    protected:
        std::shared_ptr<std::vector<types::DoubleArray>> AllocateOutputs() const;

        // members
        std::vector<std::shared_ptr<Layer>> _layers;
    };
}

#include "../tcc/Map.tcc"
