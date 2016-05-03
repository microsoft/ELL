////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Map.h (layers)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CoordinateList.h"
#include "Model.h"

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
            OutputIterator(std::vector<double> outputs);
            friend Map;
        };

        /// <summary> Constructs a map</summary>
        ///
        /// <param name="model"> A model. </param>
        /// <param name="outputCoordinates"> A list of output coordinates. </param>
        Map(const Model& model, const CoordinateList& outputCoordinateList);

        /// <summary> Computes the Map. </summary>
        ///
        /// <typeparam name="IndexValueIteratorType"> Input iterator type. </typeparam>
        /// <param name="IndexValueIterator"> The input value iterator. </param>
        ///
        /// <returns> An Iterator over output values. </returns>
        template <typename IndexValueIteratorType, linear::IsIndexValueIterator<IndexValueIteratorType> concept = 0>
        OutputIterator Compute(IndexValueIteratorType inputIterator) const;

        /// <summary> Gets the output coordinate list. </summary>
        ///
        /// <returns> The coordinate list. </returns>
        const CoordinateList& GetOutputCoordinateList() const;

        /// <summary> Gets the underlying model. </summary>
        ///
        /// <returns> The model. </returns>
        const Model& GetModel() const;

    private:
        // members
        const Model& _model;
        const CoordinateList& _outputCoordinateList;
        mutable std::vector<std::vector<double>> _layerOutputs;

        template <typename IndexValueIteratorType>
        void InitializeLayerOutputs(IndexValueIteratorType& inputIterator, std::vector<double>& layerOutputs) const;
    };
}

#include "../tcc/Map.tcc"
