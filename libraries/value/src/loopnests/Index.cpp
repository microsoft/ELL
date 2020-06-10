////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Index.cpp (value)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "loopnests/Index.h"

namespace ell
{
namespace value
{
    namespace loopnests
    {
        Index::Index(const std::string& name) :
            _name(name),
            _id(Index::GetNextId())
        {
        }

        const std::string& Index::GetName() const
        {
            return _name;
        }

        Index::Id Index::GetId() const
        {
            return _id;
        }

        // TODO: Change this so that IDs are the responsibility of the EmitterContext
        Index::Id Index::GetNextId()
        {
            static Id _nextIndex = 0;
            return _nextIndex++;
        }

        std::ostream& operator<<(std::ostream& os, const Index& index)
        {
            os << index.GetName();
            return os;
        }

    } // namespace loopnests
} // namespace value
} // namespace ell

using namespace ell::value::loopnests;

std::hash<Index>::result_type std::hash<Index>::operator()(const argument_type& element) const
{
    return static_cast<size_t>(std::hash<Index::Id>()(element.GetId()));
}
