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

#include "Layer.h"
#include "Coordinate.h"
#include "Stack.h"

// types
#include "types.h"

// stl
#include <iostream>
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
        Map(const std::shared_ptr<Stack>& layers);

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
        /// <returns> The current output coordinates. </returns>
        CoordinateList GetOutputCoordinates() const;

        /// <summary> Sets the output coordinates for the map. </summary>
        ///
        /// <param name="coordinates"> The new output coordinates. </param>
        void SetOutputCoordinates(const CoordinateList& coordinates);


        // Accessor for stack
        const Stack& GetStack() const { return *_stack; }
        Stack& GetStack() { return *_stack; }

    protected:
        // members
        std::shared_ptr<Stack> _stack;

        void IncreaseInputLayerSize(uint64 minSize) const;

        template <typename IndexValueIteratorType>
        void LoadInputLayer(IndexValueIteratorType& inputIterator, std::vector<double>& layerOutputs) const;

    private:
        // #### TODO: yuck, try to get rid of these "mutable"s
        mutable uint64 _maxInputSize = 0; // keeps track of the largest input we've seen so far
        mutable CoordinateList _outputCoordinates; // zero-size means "use all of last layer"
        mutable std::vector<std::vector<double>> _layerOutputs;
        void AllocateLayerOutputs() const;
    };
}

#include "../tcc/Map.tcc"
