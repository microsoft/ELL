////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     AutoDataVector.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataVector.h"
#include "DenseDataVector.h"
#include "TextLine.h"

// utilities
#include "Exception.h"
#include "TypeTraits.h"

// stl
#include <initializer_list>

namespace ell
{
namespace data
{
    /// <summary> Base class for DataVectors that automatically determine their internal
    /// representation. </summary>
    ///
    /// <typeparam name="DefaultDataVectorType"> The default internal representation. Datavectors are
    ///  created in this type and then possibly changed if a different type is more appropriate. </typeparam>
    template <typename DefaultDataVectorType>
    class AutoDataVectorBase : public IDataVector
    {
    public:
        AutoDataVectorBase(AutoDataVectorBase&& vector) = default;

        AutoDataVectorBase(const AutoDataVectorBase& vector) = delete;

        /// <summary> Constructs an auto data vector from a vector of the default type. </summary>
        ///
        /// <param name="vector"> The input vector. </param>
        AutoDataVectorBase(DefaultDataVectorType&& vector);

        /// <summary> Constructs an auto data vector from an index value iterator. </summary>
        ///
        /// <typeparam name="IndexValueIteratorType"> Type of index value iterator. </typeparam>
        /// <param name="IndexValueIterator"> The index value iterator. </param>
        template <typename IndexValueIteratorType, IsIndexValueIterator<IndexValueIteratorType> Concept = true>
        AutoDataVectorBase(IndexValueIteratorType indexValueIterator);

        /// <summary> Constructs a data vector from an initializer list of index value pairs. </summary>
        ///
        /// <param name="list"> The initializer list. </param>
        AutoDataVectorBase(std::initializer_list<IndexValue> list);

        /// <summary> Constructs a data vector from an initializer list of values. </summary>
        ///
        /// <param name="list"> The initializer list of values. </param>
        AutoDataVectorBase(std::initializer_list<double> list);

        /// <summary> Constructs a data vector from a vector of index value pairs. </summary>
        ///
        /// <param name="list"> The initializer list. </param>
        AutoDataVectorBase(std::vector<IndexValue> vec);

        /// <summary> Constructs a data vector from a vector of values. </summary>
        ///
        /// <param name="list"> The vector of values. </param>
        AutoDataVectorBase(std::vector<double> vec);

        /// <summary> Not Implemented. </summary>
        void AppendElement(size_t index, double value) override;

        /// <summary> Gets the data vector type. </summary>
        ///
        /// <returns> The data vector type. </returns>
        IDataVector::Type GetType() const override { return IDataVector::Type::AutoDataVector; }

        /// <summary> Gets the type of the internal data vector stored inside the auto data vector. </summary>
        ///
        /// <returns> The internal data vector type. </returns>
        IDataVector::Type GetInternalType() const { return _pInternal->GetType(); }

        /// <summary>
        /// A data vector has infinite dimension and ends with a suffix of zeros. This function returns
        /// the first index in this suffix. Equivalently, the returned value is one plus the index of the
        /// last non-zero element.
        /// </summary>
        ///
        /// <returns> The first index of the suffix of zeros at the end of this vector. </returns>
        size_t PrefixLength() const override { return _pInternal->PrefixLength(); }

        /// <summary> Computes the 2-norm of the vector. </summary>
        ///
        /// <returns> The squared 2-norm of the vector. </returns>
        double Norm2Squared() const override { return _pInternal->Norm2Squared(); }

        /// <summary> Computes the dot product with another vector. </summary>
        ///
        /// <param name="vector"> The other vector. </param>
        ///
        /// <returns> A dot product. </returns>
        double Dot(math::UnorientedConstVectorBase<double> vector) const override;

        /// <summary> Computes the dot product with another vector. </summary>
        ///
        /// <param name="vector"> The other vector. </param>
        ///
        /// <returns> A dot product. </returns>
        float Dot(math::UnorientedConstVectorBase<float> vector) const override;

        /// <summary> Adds this data vector to a math::RowVector </summary>
        ///
        /// <param name="vector"> [in,out] The vector that this DataVector is added to. </param>
        void AddTo(math::RowVectorReference<double> vector) const override;

        /// <summary>
        /// Adds a sparsely transformed version of this data vector to a math::RowVector.
        /// </summary>
        ///
        /// <typeparam name="policy"> The iteration policy. </typeparam>
        /// <typeparam name="TransformationType"> transformation type, which is a functor that takes a
        /// double and returns a double, and can be applied only to non-zeros . </typeparam>
        /// <param name="vector"> The vector. </param>
        /// <param name="transformation"> A functor that takes an IndexValue and returns a double, which is
        /// applied to each element before it is added to the vector. </param>
        template <IterationPolicy policy, typename TransformationType>
        void AddTransformedTo(math::RowVectorReference<double> vector, TransformationType transformation) const;

        /// <summary> Copies the contents of this DataVector into a double array of size PrefixLength(). </summary>
        ///
        /// <returns> The array. </returns>
        std::vector<double> ToArray() const override { return _pInternal->ToArray(); };

        /// <summary> Copies the contents of this DataVector into a double array of a given size. </summary>
        ///
        /// <param name="size"> The desired array size. </param>
        ///
        /// <returns> The array. </returns>
        std::vector<double> ToArray(size_t size) const override;

        /// <summary> Copies this data vector into another type of data vector. </summary>
        ///
        /// <typeparam name="ReturnType"> The return type. </typeparam>
        /// <typeparam name="ArgTypes"> Type of the argument. </typeparam>
        /// <param name="args"> Variable number of arguments, forwarded as is to the underlying datavector. </param>
        ///
        /// <returns> The new data vector. </returns>
        template <typename ReturnType, typename... ArgTypes>
        ReturnType CopyAs(ArgTypes... args) const;

        /// <summary> Copies a transformed version of this data vector. </summary>
        ///
        /// <typeparam name="policy"> The iteration policy. </typeparam>
        /// <typeparam name="ReturnType"> The return data vector type. </typeparam>
        /// <typeparam name="ArgTypes"> Argument types. </typeparam>
        /// <param name="args"> Arguments. </param>
        ///
        /// <returns> The transformed data vector. </returns>
        template <IterationPolicy policy, typename ReturnType, typename... ArgTypes>
        ReturnType TransformAs(ArgTypes... args) const;

        /// <summary> Human readable printout to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write to. </param>
        void Print(std::ostream& os) const override;

    private:
        // helper function used by ctors to choose the type of data vector to use
        void FindBestRepresentation(DefaultDataVectorType defaultDataVector);

        template <typename DataVectorType, utilities::IsSame<DataVectorType, DefaultDataVectorType> Concept = true>
        void SetInternal(DefaultDataVectorType defaultDataVector)
        {
            // STYLE intentional deviation from project style due to compilation difficulties
            _pInternal = std::make_unique<DefaultDataVectorType>(std::move(defaultDataVector));
        }

        template <typename DataVectorType, utilities::IsDifferent<DataVectorType, DefaultDataVectorType> Concept = true>
        void SetInternal(DefaultDataVectorType defaultDataVector);

        // members
        std::unique_ptr<IDataVector> _pInternal;
    };

    // friendly name
    using AutoDataVector = AutoDataVectorBase<DoubleDataVector>;

    /// <summary> A helper class that constructs AutoDataVectors using a provided IndexValue iterator. </summary>
    ///
    /// <typeparam name="IndexValueParsingIterator"> Parsing iterator type. </typeparam>
    template <typename IndexValueParsingIterator>
    struct AutoDataVectorParser
    {
        // The return type of the parser so the example iterator knows how to declare an Example<DataParser::type, MetadataParser::type>
        using type = AutoDataVector;

        /// <summary> Parses a given text line and constructs an AutoDataVector. </summary>
        ///
        /// <param name="textLine"> The text line. </param>
        ///
        /// <returns> An AutoDataVector. </returns>
        static AutoDataVector Parse(TextLine& textLine);
    };
}
}

#include "../tcc/AutoDataVector.tcc"