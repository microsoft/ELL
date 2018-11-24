////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Activation.h (neural)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// archiving
#include "IArchivable.h"

// math
#include "Tensor.h"
#include "Vector.h"

// stl
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

            /// <summary> Applies the activation to the input vector in-place. </summary>
            ///
            /// <param name="input"> The input value. </param>
            virtual ElementType operator()(const ElementType input) const;

            /// <summary> Applies the activation to the input vector in-place. </summary>
            ///
            /// <param name="input"> The input value. </param>
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

            /// <summary> Internal use only. </summary>
            void LegacyReadFromArchive(utilities::Unarchiver& archiver);

        private:
            std::unique_ptr<ActivationImpl<ElementType>> _impl;
        };
    } // namespace neural
} // namespace predictors
} // namespace ell

#include "../tcc/Activation.tcc"
