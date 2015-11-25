// Layer.h
#pragma once

#include "types.h"
#include "RealArray.h"
using common::RealArray;

#include <vector>
using std::vector;

#include <memory>
using std::unique_ptr;

namespace mappings
{
    class Layer
    {
    public:
        /// Ctor
        ///
        Layer(uint64 size);

        /// Default copy ctor
        ///
        Layer(const Layer& other) = default;

        /// Default move ctor
        ///
        Layer(Layer&&) = default;

        /// Initializes the idenity layer by copying the input
        ///
        template<typename IndexValueIteratorType, typename concept = enable_if_t<is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        void Set(IndexValueIteratorType indexValueIterator);

        /// Trivial implementation of Compute which does nothing - override this function in derived classes
        ///
        virtual void Compute(const vector<unique_ptr<Layer>>& previousLayers);

        /// \returns The size of the layer's output
        ///
        uint64 Size() const;

        /// \returns The output value at a given index
        ///
        double GetValue(uint64 index) const;

        /// Sets the output to zero
        ///
        void Clear();

        /// \Returns An Iterator that points to the beginning of the vector.
        ///
        RealArray<double>::Iterator GetIterator() const;

    protected:
        RealArray<double> _output;
    };
}

#include "../tcc/Layer.tcc"

