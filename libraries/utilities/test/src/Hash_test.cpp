////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Hash_test.cpp (utilities)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Hash_test.h"

#include <testing/include/testing.h>

#include <utilities/include/Hash.h>

#include <tuple>
#include <utility>

namespace ell
{

void Hash_test1()
{
    bool ok = true;
    ok &= testing::IsEqual(std::hash<int>{}(3), utilities::HashValue(3));
    {
        size_t seed = 0;
        utilities::HashCombine(seed, 3);
        ok &= testing::IsEqual(seed, utilities::HashValue(std::tuple{ 3 }));
    }
    ok &= testing::IsTrue(utilities::HashValue(std::vector{ 1, 2, 3 }) != utilities::HashValue(std::vector{ 3, 2, 1 }));
    testing::ProcessTest("Hash utility test", ok);
}

} // namespace ell
