////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     VariantDerived.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "VariantBase.h"
#include "ObjectDescription.h"

// stl
#include <memory>
#include <typeindex>
#include <string>
#include <type_traits>
#include <utility>

#include <iostream>

class ISerializable;

/// <summary> utilities namespace </summary>
namespace utilities
{
    /// <summary>
    template <typename ValueType>
    class VariantDerived : public VariantBase
    {
    public:
        VariantDerived(const ValueType& val) : VariantBase(typeid(ValueType)), _value(val) {}

        const ValueType& GetValue() const { return _value; }

        virtual std::unique_ptr<VariantBase> Clone() const override
        {
            auto ptr = static_cast<VariantBase*>(new VariantDerived<ValueType>(_value));
            return std::unique_ptr<VariantBase>(ptr);
        }

        virtual std::string ToString() const override;

        virtual std::string GetStoredTypeName() const override;

        virtual bool IsPrimitiveType() const override { return std::is_fundamental<ValueType>::value; }

        virtual bool IsSerializable() const override { return !IsPrimitiveType(); }

        virtual bool IsPointer() const override { return std::is_pointer<ValueType>::value; }

        virtual std::unique_ptr<ObjectDescription> GetObjectDescription() const override;

    private:
        ValueType _value;
    };
}

#include "../tcc/VariantDerived.tcc"
