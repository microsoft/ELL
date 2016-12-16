////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RandomEngines.h (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <random>
#include <string>
#include <vector>

namespace ell
{
namespace utilities
{
    /// <summary> Gets a vector of random engines. </summary>
    ///
    /// <param name="num"> The number of random engines to get. </param>
    /// <param name="seed_string"> The seed string. </param>
    ///
    /// <returns> The random engines. </returns>
    std::vector<std::default_random_engine> GetRandomEngines(int num = 1, std::string seed_string = "");

    /// <summary> Gets a random engine. </summary>
    ///
    /// <param name="seed_string"> The seed string. </param>
    ///
    /// <returns> The random engine. </returns>
    std::default_random_engine GetRandomEngine(std::string seed_string = "");
}
}
