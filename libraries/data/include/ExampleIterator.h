////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ExampleIterator.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "IIterator.h"
#include "StlContainerIterator.h"

namespace ell
{
namespace data
{
    /// <summary> An iterator over examples whose Get() function returns a const reference to an example. </summary>
    template <typename ExampleType>
    using ExampleReferenceIterator = utilities::VectorReferenceIterator<ExampleType>;

    /// <summary> Interface for example iterators whose Get() function returns an example (rather than a const reference). </summary>
    ///
    /// <typeparam name="ExampleType"> Example type. </typeparam>
    template <typename ExampleType>
    using IExampleIterator = typename utilities::IIterator<ExampleType>;

    /// <summary> An example iterator that wraps an IExampleIterator. </summary>
    ///
    /// <typeparam name="ExampleType"> Example type. </typeparam>
    template <typename ExampleType>
    class ExampleIterator
    {
    public:
        /// <summary> Constructs an instance of ExampleIterator. </summary>
        ///
        /// <param name="iterator"> Shared pointer to an IExampleIterator. </param>
        ExampleIterator(std::unique_ptr<IExampleIterator<ExampleType>>&& iterator);

        ExampleIterator(ExampleIterator<ExampleType>&&) = default;

        // Copy ctor deleted because this class contains a unique_ptr
        ExampleIterator(const ExampleIterator<ExampleType>&) = delete;

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if the iterator is currently pointing to a valid iterate. </returns>
        bool IsValid() const { return _iterator->IsValid(); }

        /// <summary> Proceeds to the Next iterate. </summary>
        void Next() { _iterator->Next(); }

        /// <summary> Returns the current example. </summary>
        ///
        /// <returns> An example. </returns>
        ExampleType Get() const { return _iterator->Get(); }

    private:
        std::unique_ptr<IExampleIterator<ExampleType>> _iterator;
    };

    using AutoSupervisedExampleIterator = ExampleIterator<AutoSupervisedExample>;
    using AutoSupervisedMultiClassExampleIterator = ExampleIterator<AutoSupervisedMultiClassExample>;
}
}

#include "../tcc/ExampleIterator.tcc"