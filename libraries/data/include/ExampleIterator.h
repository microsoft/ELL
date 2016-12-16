////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ExampleIterator.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace data
{
    /// <summary> Interface for example iterators. </summary>
    ///
    /// <typeparam name="ExampleType"> Example type. </typeparam>
    template <typename ExampleType>
    struct IExampleIterator
    {
        virtual ~IExampleIterator() = default;

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if the iterator is currently pointing to a valid iterate. </returns>
        virtual bool IsValid() const = 0;

        /// <summary> Proceeds to the Next iterate. </summary>
        virtual void Next() = 0;

        /// <summary> Returns the current example. </summary>
        ///
        /// <returns> An example. </returns>
        virtual ExampleType Get() const = 0;
    };

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
}
}

#include "../tcc/ExampleIterator.tcc"