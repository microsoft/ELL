////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Boolean.h (utilities)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// stl
#include <type_traits>

namespace ell
{
namespace utilities
{
/// <summary> A simple wrapper around bool to work around the std::vector<bool> specialization. </summary>
struct Boolean
{
    /// <summary> Default constructor. Constructs an instance with a wrapped false bool value. </summary>
    Boolean();

    /// <summary> Constructs an instance that wraps the provided bool value. </summary>
    /// <param name="value"> The bool value to wrap </param>
    Boolean(bool value);

    /// <summary> bool conversion operation </summary>
    operator bool() const { return value; }

private:
    bool value = false;
};
static_assert(std::is_default_constructible_v<Boolean> &&
            std::is_nothrow_move_assignable_v<Boolean> &&
            std::is_nothrow_move_constructible_v<Boolean> &&
            std::is_swappable_v<Boolean>);

bool operator==(Boolean b1, Boolean b2);

bool operator!=(Boolean b1, Boolean b2);

}
}
