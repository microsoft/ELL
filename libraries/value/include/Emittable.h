////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Emittable.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <type_traits>

namespace ell
{
namespace value
{

    /// <summary> A wrapper around void* for classes derived from EmitterContext to work with custom data </summary>
    /// <remarks> This is a type-erased wrapper around emittable values. This way, a class that derives from EmitterContext
    /// can enforce a clean separation between emittable code and the emitter implementation. </remarks>
    struct Emittable
    {
        Emittable();

        /// <summary> Sets the data to be held </summary>
        /// <param name="data"> The data to be stored in this instance </param>
        Emittable(void* data);

        /// <summary> Returns the data as the type specified </summary>
        /// <typeparam name="T"> The type to which the data should be casted </typeparam>
        template <typename T>
        T GetDataAs() const
        {
            return reinterpret_cast<T>(_data);
        }

    private:
        void* _data = nullptr;
    };
    static_assert(std::is_default_constructible_v<Emittable> && std::is_nothrow_move_assignable_v<Emittable> &&
                  std::is_nothrow_move_constructible_v<Emittable> && std::is_swappable_v<Emittable>);

} // namespace value
} // namespace ell
