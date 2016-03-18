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

// stl
#include <iostream>
#include <vector>
#include <memory>
#include <type_traits>

namespace layers
{
    /// <summary> Implements a layer stack. </summary>
    class LayerStack
    {
    public:

        /// <summary> Constructs an instance of Map. </summary>
        LayerStack();

        /// <summary> Deleted copy constructor </summary>
        LayerStack(const LayerStack&) = delete;

        /// <summary> Default move constructor </summary>
        LayerStack(LayerStack&&) = default;

        /// <summary> Virtual destructor. </summary>
        virtual ~LayerStack() = default;


        /// <summary> Adds a layer to the map. </summary>
        ///
        /// <param name="layer"> The layer to add to the map. </param>
        ///
        /// <returns> The index of the added layer. </returns>
        uint64 AddLayer(std::unique_ptr<Layer>&& layer);

        /// <summary> Returns the number of layers in the map. </summary>
        ///
        /// <returns> The total number of layers in the map. </returns>
        uint64 NumLayers() const;

        /// <summary> Gets a Layer cast as a specified layer type, used when derived classes add functionality to layers </summary>
        ///
        /// <typeparam name="LayerType"> Layer type to return. </typeparam>
        /// <param name="layerIndex"> Zero-based index of the layer. </param>
        ///
        /// <returns> The requested layer, cast to a const reference of the requested type. </returns>
        template <typename LayerType = Layer>
        const LayerType& GetLayer(uint64 layerIndex) const;

        /// <summary> Static function that loads a Map from file. </summary>
        ///
        /// <typeparam name="MapType"> Map type to load. </typeparam>
        /// <param name="inputMapFile"> Name of the map file to load. </param>
        ///
        /// <returns> A MapType. </returns>
        template<typename MapType = LayerStack>
        static MapType Load(const std::string& inputMapFile);

        /// <summary> Saves a map to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        void Save(std::ostream& os) const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static const char* GetTypeName();

        /// <summary> Reads the map from an XMLDeserializer. </summary>
        ///
        /// <param name="deserializer"> [in,out] The deserializer. </param>
        void Read(utilities::XMLDeserializer& deserializer);

        /// <summary> Writes the map to an XMLSerializer. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        void Write(utilities::XMLSerializer& serializer) const;

    protected:
        // members
        std::vector<std::unique_ptr<Layer>> _layers;

        void IncreaseInputLayerSize(uint64 minSize) const;

    private:
        static const int _currentVersion = 1;
    };


    /// <summary> Implements a map. </summary>
    class Map
    {
    public:

        /// <summary> An iterator over the output values of the map. </summary>
        class OutputIterator : public IIndexValueIterator
        {
        public:
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
            std::vector<double> _outputs;
            uint64 _index;

            // private ctor, can only be called from Map class
            OutputIterator(std::vector<double>&& outputs);
            friend Map;
        };

        /// <summary> Constructs an instance of Map. </summary>
        Map();

        /// <summary> Constructs an instance of pointing to an existing stack of layers. </summary>
        Map(const std::shared_ptr<LayerStack>& layers);

        /// <summary> Deleted copy constructor </summary>
        Map(const Map&) = delete;

        /// <summary> Default move constructor </summary>
        Map(Map&&) = default;

        /// <summary> Virtual destructor. </summary>
        virtual ~Map() = default;

        /// <summary> Computes the Map. </summary>
        ///
        /// <typeparam name="IndexValueIteratorType"> Input iterator type. </typeparam>
        /// <param name="IndexValueIterator"> The input value iterator. </param>
        ///
        /// <returns> An Iterator over output values. </returns>
        template <typename IndexValueIteratorType, typename concept = std::enable_if_t<std::is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        OutputIterator Compute(IndexValueIteratorType inputIterator) const;

        /// <summary> Returns the current output coordinates for the map. </summary>
        ///
        /// <returns> The currently-set output coordinates. </returns>
        CoordinateList GetOutputCoordinates() const;

        /// <summary> Sets the output coordinates for the map. </summary>
        ///
        /// <param name="coordinates"> The currently-set output coordinates. </param>
        void SetOutputCoordinates(const CoordinateList& coordinates);

    protected:
        // members
        std::shared_ptr<LayerStack> _layerStack;

        void IncreaseInputLayerSize(uint64 minSize) const;

        template <typename IndexValueIteratorType>
        void LoadInputLayer(IndexValueIteratorType& inputIterator, std::vector<double>& layerOutputs) const;

    private:
        mutable uint64 _maxInputSize = 0;
        mutable CoordinateList _outputCoordinates; // zero-size means "use all of last layer"
        std::vector<std::vector<double>> AllocateLayerOutputs() const;
    };
}

#include "../tcc/Map.tcc"
