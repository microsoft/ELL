////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Reference.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Value.h"

namespace ell
{
namespace value
{
    class Value;

    template <typename ViewType>
    class Ref;

    template <>
    class Ref<Value>
    {
    public:
        Ref(Value);

        Ref(std::in_place_t, Value);

        Ref(const Ref&);
        Ref(Ref&&) noexcept;
        Ref& operator=(const Ref&);
        Ref& operator=(Ref&&);
        ~Ref();

        Value operator*() const;

        Value GetValue() const;

        void SetName(const std::string& name);
        std::string GetName() const;

    private:
        Value _value;
    };

    template <typename ViewType>
    class Ref : private Ref<Value>
    {
    public:
        using Ref<Value>::Ref;

        Ref(ViewType view);
        Ref(std::in_place_t, ViewType view);

        Ref(const Ref&);
        Ref(Ref&&) noexcept;
        Ref& operator=(const Ref&);
        Ref& operator=(Ref&&);
        ~Ref();

        ViewType operator*() const;

        using Ref<Value>::GetValue;
        using Ref<Value>::SetName;
        using Ref<Value>::GetName;
    };

    using Reference = Ref<Value>;

} // namespace value
} // namespace ell

#pragma region implementation
namespace ell
{
namespace value
{
    namespace detail
    {
        template <typename T>
        struct IsRef : std::false_type
        {};

        template <typename ViewType>
        struct IsRef<::ell::value::Ref<ViewType>> : std::true_type
        {};

    } // namespace detail

    template <typename ViewType>
    Ref<ViewType>::Ref(ViewType view) :
        Ref<Value>::Ref(view.GetValue())
    {}

    template <typename ViewType>
    Ref<ViewType>::Ref(std::in_place_t, ViewType view) :
        Ref<Value>::Ref(std::in_place, view.GetValue())
    {}

    template <typename ViewType>
    Ref<ViewType>::Ref(const Ref& other) :
        Ref<Value>::Ref(other)
    {}

    template <typename ViewType>
    Ref<ViewType>::Ref(Ref&& other) noexcept :
        Ref<Value>::Ref(std::move(other))
    {}

    template <typename ViewType>
    Ref<ViewType>& Ref<ViewType>::operator=(const Ref& other)
    {
        (void)Ref<Value>::operator=(other);
        return *this;
    }

    template <typename ViewType>
    Ref<ViewType>& Ref<ViewType>::operator=(Ref&& other)
    {
        (void)Ref<Value>::operator=(std::move(other));
        return *this;
    }

    template <typename ViewType>
    Ref<ViewType>::~Ref() = default;

    template <typename ViewType>
    ViewType Ref<ViewType>::operator*() const
    {
        if constexpr (detail::IsRef<ViewType>{})
        {
            return std::make_from_tuple<ViewType>(std::make_tuple(std::in_place, Ref<Value>::operator*()));
        }
        else
        {
            return std::make_from_tuple<ViewType>(std::make_tuple(Ref<Value>::operator*()));
        }
    }
} // namespace value
} // namespace ell
#pragma endregion implementation
