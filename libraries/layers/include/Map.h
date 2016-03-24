////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Map.h (layers)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Coordinate.h"
#include "Stack.h"

// linear
#include "IndexValue.h"

// stl
#include <cstdint>
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
        class OutputIterator : public linear::IIndexValueIterator
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
            linear::IndexValue Get() const;

        protected:
            std::vector<double> _outputs;
            uint64_t _index;

            // private ctor, can only be called from Map class
            OutputIterator(std::vector<double>&& outputs);
            friend Map;
        };

        /// <summary> Constructs an instance of Map. </summary>
        Map();

        /// <summary> Constructs an instance of pointing to an existing stack of layers, and using the full last layer as output. </summary>
        Map(const std::shared_ptr<Stack>& layers);

        /// <summary> Constructs an instance of pointing to an existing stack of layers, using the given output coordinates. </summary>
        Map(const std::shared_ptr<Stack>& layers, const CoordinateList& outputCoordinates);

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
        template <typename IndexValueIteratorType, linear::IsIndexValueIterator<IndexValueIteratorType> concept = 0>
        OutputIterator Compute(IndexValueIteratorType inputIterator) const;

        /// <summary> Returns the current output coordinates for the map. </summary>
        ///
        /// <returns> The current output coordinates. </returns>
        CoordinateList GetOutputCoordinates() const;

        /// <summary> Sets the output coordinates for the map. </summary>
        ///
        /// <param name="coordinates"> The new output coordinates. </param>
        void SetOutputCoordinates(const CoordinateList& coordinates);

        /// <summary> Gets a const reference to the layer stack this map refers to </summary>
        const Stack& GetStack() const;

        /// <summary> Gets a non-const reference to the layer stack this map refers to </summary>
        Stack& GetStack();

    protected:
        // members
        std::shared_ptr<Stack> _stack;

        void IncreaseInputLayerSize(uint64_t minSize) const;

        template <typename IndexValueIteratorType>
        void LoadInputLayer(IndexValueIteratorType& inputIterator, std::vector<double>& layerOutputs) const;

    private:
        CoordinateList _outputCoordinates; // default empty coordinate list means "use all of last layer"

        mutable uint64_t _maxInputSizeSeen = 0; // keeps track of the largest input we've seen so far
        mutable std::vector<std::vector<double>> _layerOutputs;
        void AllocateLayerOutputs() const;
    };
}

#include "../tcc/Map.tcc"
