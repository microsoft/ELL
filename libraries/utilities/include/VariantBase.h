////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     VariantBase.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Exception.h"
#include "TypeName.h"

// stl
#include <memory>
#include <typeindex>
#include <string>
#include <type_traits>
#include <utility>

class ISerializable;

/// <summary> utilities namespace </summary>
namespace utilities
{
    class ObjectDescription;
    
    // Forward declaration of derived class
    template <typename ValueType>
    class VariantDerived;

    /// <summary> Base class for value holder </summary>
    class VariantBase
    {
    public:
        VariantBase(std::type_index type) : _type(type){};
        virtual ~VariantBase() = default;

        template <typename ValueType>
        ValueType GetValue() const
        {
            auto thisPtr = dynamic_cast<const VariantDerived<ValueType>*>(this);
            if (thisPtr == nullptr)
            {
                throw InputException(InputExceptionErrors::typeMismatch, "Variant::GetValue called with wrong type.");
            }

            return thisPtr->GetValue();
        }

        virtual std::unique_ptr<VariantBase> Clone() const = 0;

        virtual std::string ToString() const = 0;

        virtual std::string GetStoredTypeName() const = 0;

        virtual bool IsPrimitiveType() const = 0;

        virtual bool IsSerializable() const = 0;

        virtual bool IsPointer() const = 0;

        virtual std::unique_ptr<ObjectDescription> GetObjectDescription() const = 0;
    private:
        std::type_index _type; // redundant with type in Variant class.
    };
}
