////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CodePositionConstraints.cpp (value)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "loopnests/CodePositionConstraints.h"

#include <utilities/include/Hash.h>

#include <stdexcept>

namespace ell
{
namespace value
{
    namespace loopnests
    {
        CodePositionConstraints::CodePositionConstraints(LoopFragmentType placement, std::vector<Index> requiredIndices, std::vector<Index> boundaryIndices) :
            _placement(placement),
            _requiredIndices(requiredIndices),
            _boundaryIndices(boundaryIndices)
        {
        }

        std::vector<Index> CodePositionConstraints::GetRequiredIndices() const
        {
            return _requiredIndices;
        }

        std::vector<Index> CodePositionConstraints::GetBoundaryIndices() const
        {
            return _boundaryIndices;
        }

        std::ostream& operator<<(std::ostream& os, LoopFragmentType t)
        {
            switch (t)
            {
            case LoopFragmentType::prologue:
                os << "prologue";
                break;
            case LoopFragmentType::body:
                os << "body";
                break;
            case LoopFragmentType::boundary:
                os << "boundary";
                break;
            case LoopFragmentType::epilogue:
                os << "epilogue";
                break;
            case LoopFragmentType::LAST:
                os << "LAST";
                break;
            default:
                throw std::runtime_error("Unknown enum value");
            }
            return os;
        }

        std::ostream& operator<<(std::ostream& os, LoopFragmentFlags f)
        {
            std::string sep = "";
            os << "[";
            for (int i = 0; i < static_cast<int>(LoopFragmentType::LAST); ++i)
            {
                if (f.GetFlag(static_cast<LoopFragmentType>(i)))
                {
                    os << sep << LoopFragmentType(i);
                    sep = " | ";
                }
            }
            os << "]";
            return os;
        }

        bool operator==(const CodePositionConstraints& i1, const CodePositionConstraints& i2)
        {
            return (i1.GetPlacement() == i2.GetPlacement()) && (i1.GetRequiredIndices() == i2.GetRequiredIndices()) && (i1.GetBoundaryIndices() == i2.GetBoundaryIndices());
        }

        bool operator!=(const CodePositionConstraints& i1, const CodePositionConstraints& i2)
        {
            return !(i1 == i2);
        }
    } // namespace loopnests
} // namespace value
} // namespace ell

using namespace ell::value::loopnests;

std::hash<CodePositionConstraints>::result_type std::hash<CodePositionConstraints>::operator()(const argument_type& constraints) const
{
    using ::ell::utilities::HashCombine;

    size_t hash = 0;
    HashCombine(hash, constraints.GetPlacement());
    HashCombine(hash, constraints.GetRequiredIndices());
    HashCombine(hash, constraints.GetBoundaryIndices());

    return hash;
}
