////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Index.h (value)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ostream>
#include <string>

namespace ell
{
namespace value
{
    namespace loopnests
    {
        /// <summary>
        /// A placeholder object representing a runtime variable used as the index for a loop (e.g., the 'i' in 'for(i = ...)').
        /// </summary>
        class Index
        {
        public:
            using Id = int;
            Index() = default;
            Index(const Index& other) = default;
            Index(Index&& other) = default;
            Index(const std::string& name);
            Index& operator=(const Index& other) = default;
            Index& operator=(Index&& other) = default;

            const std::string& GetName() const;
            Id GetId() const;

        private:
            static int GetNextId();

            friend inline bool operator==(const Index& i1, const Index& i2) { return i1.GetId() == i2.GetId(); }
            friend inline bool operator!=(const Index& i1, const Index& i2) { return !(i1 == i2); }
            friend inline bool operator<(const Index& i1, const Index& i2) { return i1.GetId() < i2.GetId(); }

            std::string _name;
            Id _id = -1;
        };

        struct SplitIndex
        {
            Index outer;
            Index inner;
        };

        std::ostream& operator<<(std::ostream& os, const Index& index);
    } // namespace loopnests
} // namespace value
} // namespace ell

namespace std
{
template <>
struct hash<::ell::value::loopnests::Index>
{
    using argument_type = ::ell::value::loopnests::Index;
    using result_type = std::size_t;
    result_type operator()(const argument_type& index) const;
};
} // namespace std
