////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CodePositionConstraints.h (value)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Index.h"

#include <bitset>
#include <ostream>
#include <vector>

namespace ell
{
namespace value
{
    namespace loopnests
    {
        enum class LoopFragmentType : int
        {
            prologue, // Occurs as part of a loop prologue, before the main loop body
            body, // Occurs as part of the main loop body
            boundary, // Occurs as part of a loop boundary condition
            epilogue, // Occurs as part of a loop epilogue, after the main loop body
            LAST
        };

        bool IsBoundaryFragment(LoopFragmentType f);

        std::ostream& operator<<(std::ostream& os, LoopFragmentType t);

        // A set of LoopFragmentTypes
        class LoopFragmentFlags
        {
        public:
            LoopFragmentFlags() = default;
            LoopFragmentFlags(LoopFragmentType type)
            {
                _flags[static_cast<int>(type)] = 1;
            };
            explicit LoopFragmentFlags(int flags);

            bool GetFlag(LoopFragmentType type) const
            {
                return _flags[static_cast<int>(type)];
            }

            void SetFlag(LoopFragmentType type, bool value)
            {
                _flags[static_cast<int>(type)] = value;
            }

            static LoopFragmentFlags All()
            {
                LoopFragmentFlags result;
                result.SetFlag(LoopFragmentType::prologue, true);
                result.SetFlag(LoopFragmentType::body, true);
                result.SetFlag(LoopFragmentType::boundary, false);
                result.SetFlag(LoopFragmentType::epilogue, true);
                return result;
            }

            LoopFragmentFlags& operator&=(const LoopFragmentFlags& other)
            {
                _flags &= other._flags;
                return *this;
            }

            LoopFragmentFlags& operator|=(const LoopFragmentFlags& other)
            {
                _flags |= other._flags;
                return *this;
            }

        private:
            std::bitset<static_cast<int>(LoopFragmentType::LAST)> _flags;
        };

        std::ostream& operator<<(std::ostream& os, LoopFragmentFlags f);

        inline LoopFragmentFlags operator&(LoopFragmentType a, LoopFragmentType b)
        {
            LoopFragmentFlags result(a);
            result &= b;
            return result;
        }

        inline LoopFragmentFlags operator&(LoopFragmentFlags a, LoopFragmentType b)
        {
            a &= b;
            return a;
        }

        inline LoopFragmentFlags operator&(LoopFragmentType a, LoopFragmentFlags b)
        {
            return b & a;
        }

        inline LoopFragmentFlags operator|(LoopFragmentType a, LoopFragmentType b)
        {
            LoopFragmentFlags result(a);
            result |= b;
            return result;
        }

        inline LoopFragmentFlags operator|(LoopFragmentFlags a, LoopFragmentType b)
        {
            a |= b;
            return a;
        }

        inline LoopFragmentFlags operator|(LoopFragmentType a, LoopFragmentFlags b)
        {
            return b | a;
        }

        /// <summary>
        /// A class to hold the constraints that govern where a piece of code may / must run. Used to generate a concrete
        /// schedule for running (non-"kernel") code
        /// </summary>

        // TODO: each boundary index needs its own "placement" value (e.g., you could have a kernel that runs when j==0 and k==N-1)
        class CodePositionConstraints
        {
        public:
            CodePositionConstraints(LoopFragmentType placement, std::vector<Index> requiredIndices, std::vector<Index> boundaryIndices);

            LoopFragmentType GetPlacement() const { return _placement; }

            std::vector<Index> GetRequiredIndices() const; // indices we depend on
            std::vector<Index> GetBoundaryIndices() const; // indices defining the fragment

        private:
            LoopFragmentType _placement;
            std::vector<Index> _requiredIndices;
            std::vector<Index> _boundaryIndices;
        };

        bool operator==(const CodePositionConstraints& i1, const CodePositionConstraints& i2);
        bool operator!=(const CodePositionConstraints& i1, const CodePositionConstraints& i2);
    } // namespace loopnests
} // namespace value
} // namespace ell

//
// Custom specialization of std::hash so we can keep constraints in containers that require hashable types
//
namespace std
{
/// <summary> Implements a hash function for the CodePositionConstraints class, so that it can be used with associative containers (maps, sets, and the like). </summary>
template <>
struct hash<::ell::value::loopnests::CodePositionConstraints>
{
    using argument_type = ell::value::loopnests::CodePositionConstraints;
    using result_type = std::size_t;

    /// <summary> Computes a hash of the input value. </summary>
    ///
    /// <returns> A hash value for the given input. </returns>
    result_type operator()(const argument_type& constraints) const;
};
} // namespace std
