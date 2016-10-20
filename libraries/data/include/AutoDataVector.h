////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AutoDataVector.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataVector.h"
#include "DenseDataVector.h"

// utilities
#include "Exception.h"
#include "TypeTraits.h"

// stl
#include <initializer_list>

namespace emll
{
namespace data
{
    /// <summary> Base class for DataVectors that automatically determine their internal
    /// representation. </summary>
    ///
    /// <typeparam name="DefaultDataVectorType"> The default internal representation. Datavectors are
    ///  created in this type and then possibly changed if a different type is more appropriate. </typeparam>
    template<typename DefaultDataVectorType>
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
        template<typename IndexValueIteratorType, IsIndexValueIterator<IndexValueIteratorType> Concept = true>
        AutoDataVectorBase(IndexValueIteratorType indexValueIterator);

        /// <summary> Constructs a data vector from an initializer list of index value pairs. </summary>
        ///
        /// <param name="list"> The initializer list. </param>
        AutoDataVectorBase(std::initializer_list<IndexValue> list);

        /// <summary> Constructs a data vector from an initializer list of values. </summary>
        ///
        /// <param name="list"> The initializer list of values. </param>
        AutoDataVectorBase(std::initializer_list<double> list);

        /// <summary> Not Implemented. </summary>
        virtual void AppendElement(size_t index, double value) override;

        /// <summary> Gets the data vector type. </summary>
        ///
        /// <returns> The data vector type. </returns>
        virtual IDataVector::Type GetType() const override { return IDataVector::Type::AutoDataVector; }

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
        virtual size_t PrefixLength() const override { return _pInternal->PrefixLength(); }

        /// <summary> Computes the 2-norm of the vector (not the squared 2-norm). </summary>
        ///
        /// <returns> The vector 2-norm. </returns>
        virtual double Norm2() const override { return _pInternal->Norm2(); }

        /// <summary> Computes the dot product with another vector. </summary>
        ///
        /// <param name="vector"> The other vector. </param>
        ///
        /// <returns> A dot product. </returns>
        virtual double Dot(const math::UnorientedConstVectorReference<double>& vector) const override;

        /// <summary>
        /// Performs the operation: vector += scalar * (*this), where other is an array of doubles.
        /// </summary>
        ///
        /// <param name="vector"> [in,out] The vector that this DataVector is added to. </param>
        /// <param name="scalar"> The scalar. </param>
        virtual void AddTo(math::RowVectorReference<double>& vector, double scalar = 1.0) const override;

        /// <summary> Copies the contents of this DataVector into a double array. </summary>
        ///
        /// <returns> The array. </returns>
        virtual std::vector<double> ToArray() const override { return _pInternal->ToArray(); }

        /// <summary> Copies this data vector into another type of data vector. </summary>
        ///
        /// <typeparam name="ReturnType"> The return type. </typeparam>
        /// <typeparam name="ArgTypes"> Type of the argument. </typeparam>
        /// <param name="args"> Variable number of arguments, forwarded as is to the underlying datavector. </param>
        ///
        /// <returns> The new data vector. </returns>
        template<typename ReturnType, typename ... ArgTypes>
        ReturnType DeepCopyAs(ArgTypes ... args) const;

        /// <summary> Human readable printout to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write to. </param>
        virtual void Print(std::ostream & os) const override;

    private:
        // helper function used by ctors to choose the type of data vector to use
        void FindBestRepresentation(DefaultDataVectorType defaultDataVector);

        template<typename DataVectorType, utilities::IsSame<DataVectorType, DefaultDataVectorType> Concept = true>
        void SetInternal(DefaultDataVectorType defaultDataVector);

        template<typename DataVectorType, utilities::IsDifferent<DataVectorType, DefaultDataVectorType> Concept = true>
        void SetInternal(DefaultDataVectorType defaultDataVector);

        // members
        std::unique_ptr<IDataVector> _pInternal;
    };

    // friendly name
    using AutoDataVector = AutoDataVectorBase<DoubleDataVector>;
}
}

#include "../tcc/AutoDataVector.tcc"