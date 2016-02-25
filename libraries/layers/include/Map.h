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

// common
#include "types.h"
#include "RealArray.h"

// stl
#include <vector>
#include <memory>
#include <type_traits>

namespace layers
{
    /// <summary> Implemenets a map. </summary>
    class Map
    {
    public:

        class Iterator : public IIndexValueIterator
        {
        public:

            /// <summary> Copy constructor. </summary>
            ///
            /// <param name="parameter1"> The first parameter. </param>
            Iterator(const Iterator&) = default;

            /// <summary> Move constructor. </summary>
            ///
            /// <param name="parameter1"> [in,out] The first parameter. </param>
            Iterator(Iterator&&) = default;

            /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
            ///
            /// <returns> true if valid, false if not. </returns>
            bool IsValid() const;

            /// <summary> Proceeds to the Next iterate. </summary>
            void Next();

            /// <summary> \returns The current index-value pair. </summary>
            ///
            /// <returns> An IndexValue. </returns>
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
        /// <param name="inputLayerSize"> Size of the input layer. </param>
        Map(uint64 inputLayerSize);

        /// <summary> Destructor. </summary>
        virtual ~Map() = default;

        /// <summary> Computes the Map. </summary>
        ///
        /// <typeparam name="dexValueIteratorType"> Type of the dex value iterator type. </typeparam>
        /// <typeparam name="IIndexValueIterator"> Type of the index value iterator. </typeparam>
        /// <typeparam name="dexValueIteratorType"> Type of the dex value iterator type. </typeparam>
        /// <param name="IndexValueIterator"> The index value iterator. </param>
        /// <param name="outputCoordinates"> The output coordinates. </param>
        ///
        /// <returns> An Iterator. </returns>
        template <typename IndexValueIteratorType, typename concept = std::enable_if_t<std::is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        Iterator Compute(IndexValueIteratorType IndexValueIterator, const CoordinateList& outputCoordinates) const;

        /// <summary> Adds a shared layer to the map. </summary>
        ///
        /// <param name="layer"> The layer. </param>
        ///
        /// <returns> The index of the added layer. </returns>
        uint64 PushBack(std::shared_ptr<Layer> layer);

        /// <summary> Returns the number of layers in the map. </summary>
        ///
        /// <returns> The total number of layers. </returns>
        uint64 NumLayers() const;

        /// <summary> Gets a Layer of a specified template type. </summary>
        ///
        /// <typeparam name="LayerType"> Type of the layer to return. </typeparam>
        /// <param name="layerIndex"> Zero-based index of the layer. </param>
        ///
        /// <returns> The layer. </returns>
        template<typename LayerType = Layer>
        std::shared_ptr<const LayerType> GetLayer(uint64 layerIndex) const;

        /// <summary> Static function that loads a Map from file. </summary>
        ///
        /// <typeparam name="MapType"> Type of the map type. </typeparam>
        /// <param name="inputMapFile"> The input map file. </param>
        ///
        /// <returns> A MapType. </returns>
        template<typename MapType = Map>
        static MapType Load(const std::string& inputMapFile);

        /// <summary> Returns a list of the coordinates in this layer. </summary>
        ///
        /// <param name="layerIndex"> Zero-based index of the layer. </param>
        ///
        /// <returns> The coordinate list. </returns>
        CoordinateList GetCoordinateList(uint64 layerIndex) const;

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

        /// <summary> Static function for deserializing std::shared_ptr<Layer> </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        /// <param name="up"> [in,out] The up. </param>
        static void DeserializeLayers(utilities::JsonSerializer& serializer, std::shared_ptr<Layer>& up);

    protected:
        std::shared_ptr<std::vector<types::DoubleArray>> AllocateOutputs() const;

        // members
        std::vector<std::shared_ptr<Layer>> _layers;
    };
}

#include "../tcc/Map.tcc"
