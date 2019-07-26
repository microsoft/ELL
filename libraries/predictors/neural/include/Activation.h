////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Activation.h (neural)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <utilities/include/IArchivable.h>

#include <nodes/include/ActivationFunctions.h>

#include <math/include/Tensor.h>
#include <math/include/Vector.h>

#include <value/include/Scalar.h>
#include <value/include/Vector.h>

#include <memory>

namespace ell
{
namespace predictors
{
    namespace neural
    {

        /// <summary> Interface for all types of activation. </summary>
        template <typename ElementType>
        class ActivationImpl : public utilities::IArchivable
        {
        public:
            virtual ~ActivationImpl() {}

            /// <summary> Returns the output as a function of the input. </summary>
            ///
            /// <param name="input"> The input value. </param>
            ///
            /// <returns> The computed output. </param>
            virtual ElementType Apply(const ElementType input) const = 0;

            /// <summary> Returns the output as a function of the input. </summary>
            ///
            /// <param name="input"> The input value as a value library Scalar. </param>
            ///
            /// <returns> The computed output. </param>
            virtual value::Scalar Apply(value::Scalar input) const = 0;

            /// <summary> Make a copy of this activation. </summary>
            ///
            /// <returns> The copy in a unique pointer. </param>
            virtual std::unique_ptr<ActivationImpl<ElementType>> Copy() const = 0;

            /// <summary> Applies the activation to the input vector in-place. </summary>
            ///
            /// <param name="input"> The input value. </param>
            virtual ElementType operator()(const ElementType input) const;

            /// <summary> Returns the output as a function of the input. </summary>
            ///
            /// <param name="input"> The input value. </param>
            /// <param name="index"> The input index. </param>
            ///
            /// <returns> The computed output. </param>
            virtual ElementType ApplyIndex(const ElementType input, const math::IntegerTriplet& index) const;

            /// <summary> Archives this object. </summary>
            ///
            /// <param name="archiver"> The archiver. </param>
            void WriteToArchive(utilities::Archiver& /*archiver*/) const override{};

            /// <summary> Unarchives this object. </summary>
            ///
            /// <param name="archiver"> The unarchiver. </param>
            void ReadFromArchive(utilities::Unarchiver& /*archiver*/) override{};

            /// <summary> Gets the name of this type. </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("ActivationImpl"); }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            std::string GetRuntimeTypeName() const override { return GetTypeName(); }
        };

        /// <summary> A convenience wrapper for the Activation functions. This is here just to hide the std::unique_ptr implementation.</summary>
        template <typename ElementType>
        class Activation : public utilities::IArchivable
        {
        public:
            /// <summary> default constructor (only used for serialization). </summary>
            Activation() = default;

            /// <summary> Construct new Activation given an implementation, this constructor takes that implementation using std::move. </summary>
            ///
            /// <param name="impl"> The implementation. </param>
            Activation(std::unique_ptr<ActivationImpl<ElementType>>& impl);

            /// <summary> Construct new Activation given an implementation, this constructor takes ownership of the object. </summary>
            ///
            /// <param name="impl"> The implementation. </param>
            Activation(ActivationImpl<ElementType>* impl);

            /// <summary> Copy the activation and it's implementation. </summary>
            ///
            /// <param name="impl"> The activation to copy. </param>
            Activation(const Activation<ElementType>& other);

            /// <summary> Copy the activation and it's implementation. </summary>
            ///
            /// <param name="impl"> The activation to copy. </param>
            virtual Activation<ElementType>& operator=(const Activation<ElementType>& other);

            /// <summary> Returns the output as a function of the input. </summary>
            ///
            /// <param name="input"> The input value. </param>
            ///
            /// <returns> The computed output. </param>
            virtual ElementType Apply(const ElementType input) const;

            /// <summary> Returns the output as a function of the input. </summary>
            ///
            /// <param name="input"> The input value as a value library Scalar. </param>
            ///
            /// <returns> The computed output. </param>
            virtual value::Scalar Apply(value::Scalar input) const;

            /// <summary> Applies the activation to the input vector in-place. </summary>
            ///
            /// <param name="input"> The input value. </param>
            virtual ElementType operator()(const ElementType input) const;

            /// <summary> Applies the activation to the input vector in-place. </summary>
            ///
            /// <param name="input"> The input vector. </param>
            virtual void Apply(value::Vector input) const;

            /// <summary> Applies the activation to the input vector in-place. </summary>
            ///
            /// <param name="input"> The input vector. </param>
            virtual void Apply(math::ColumnVector<ElementType>& input) const;

            /// <summary> Returns the output as a function of the input. </summary>
            ///
            /// <param name="input"> The input value. </param>
            /// <param name="index"> The input index. </param>
            ///
            /// <returns> The computed output. </param>
            virtual ElementType ApplyIndex(const ElementType input, const math::IntegerTriplet& index) const;

            /// <summary> Archives this object. </summary>
            ///
            /// <param name="archiver"> The archiver. </param>
            void WriteToArchive(utilities::Archiver& /*archiver*/) const override;

            /// <summary> Unarchives this object. </summary>
            ///
            /// <param name="archiver"> The unarchiver. </param>
            void ReadFromArchive(utilities::Unarchiver& /*archiver*/) override;

            /// <summary> Gets the name of this type. </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("Activation"); }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            std::string GetRuntimeTypeName() const override { return GetTypeName(); }

            /// <summary> Internal use only. </summary>
            ActivationImpl<ElementType>* GetImpl() const { return _impl.get(); }

            /// <summary> Internal use only. </summary>
            void Reset(ActivationImpl<ElementType>* impl) { _impl.reset(impl); }

        private:
            std::unique_ptr<ActivationImpl<ElementType>> _impl;
        };
    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma region implementation

#include <algorithm>
#include <limits>

namespace ell
{
namespace predictors
{
    namespace neural
    {
        template <typename ElementType>
        ElementType ActivationImpl<ElementType>::operator()(const ElementType input) const
        {
            return Apply(input);
        }

        template <typename ElementType>
        ElementType ActivationImpl<ElementType>::ApplyIndex(const ElementType input, const math::IntegerTriplet& /*index*/) const
        {
            return Apply(input);
        }

        template <typename ElementType>
        Activation<ElementType>::Activation(std::unique_ptr<ActivationImpl<ElementType>>& impl) :
            _impl(std::move(impl))
        {}

        template <typename ElementType>
        Activation<ElementType>::Activation(ActivationImpl<ElementType>* impl) :
            _impl(impl)
        {
        }

        template <typename ElementType>
        Activation<ElementType>::Activation(const Activation<ElementType>& other) :
            _impl(std::move(other._impl->Copy()))
        {}

        template <typename ElementType>
        Activation<ElementType>& Activation<ElementType>::operator=(const Activation<ElementType>& other)
        {
            if (this != &other)
            {
                auto temp = other._impl->Copy();
                _impl.swap(temp);
            }
            return *this;
        }

        template <typename ElementType>
        ElementType Activation<ElementType>::Apply(const ElementType input) const
        {
            return _impl->Apply(input);
        }

        template <typename ElementType>
        value::Scalar Activation<ElementType>::Apply(value::Scalar input) const
        {
            return _impl->Apply(input);
        }

        template <typename ElementType>
        ElementType Activation<ElementType>::operator()(const ElementType input) const
        {
            return _impl->Apply(input);
        }

        template <typename ElementType>
        ElementType Activation<ElementType>::ApplyIndex(const ElementType input, const math::IntegerTriplet& index) const
        {
            return _impl->ApplyIndex(input, index);
        }

        template <typename ElementType>
        void Activation<ElementType>::Apply(math::ColumnVector<ElementType>& input) const
        {
            input.Transform([this](ElementType value) { return _impl->Apply(value); });
        }

        template <typename ElementType>
        void Activation<ElementType>::Apply(value::Vector input) const
        {
            For(input, [&](value::Scalar index) {
                input[index] = _impl->Apply(input[index]);
            });
        }

        template <typename ElementType>
        void Activation<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
        {
            archiver["activation"] << _impl;
        }

        template <typename ElementType>
        void Activation<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
        {
            archiver["activation"] >> _impl;
        }

        //
        // Helper function (cannot be used for ParametricReLUActivations because that has two arguments to Compute).
        //
        template <typename ValueType>
        std::unique_ptr<ell::nodes::ActivationFunction<ValueType>> GetNodeActivationFunction(const Activation<ValueType>& f);

    } // namespace neural

} // namespace predictors
} // namespace ell

#pragma endregion implementation
