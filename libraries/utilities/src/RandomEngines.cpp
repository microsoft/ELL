////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RandomEngines.cpp (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "RandomEngines.h"

namespace ell
{
namespace utilities
{
    std::vector<std::default_random_engine> GetRandomEngines(int num, std::string seed_string)
    {
        std::vector<int> seeds(num);

        if (seed_string != "")
        {
            std::seed_seq seq(seed_string.begin(), seed_string.end());
            seq.generate(seeds.begin(), seeds.end());
        }
        else
        {
            std::random_device d;
            std::seed_seq seq{ d(), d(), d(), d(), d(), d(), d(), d() };
            seq.generate(seeds.begin(), seeds.end());
        }

        // create the independent generators
        std::vector<std::default_random_engine> rngs;
        rngs.reserve(num);
        for (auto seed : seeds)
        {
            rngs.emplace_back(seed);
        }

        return rngs;
    }

    std::default_random_engine GetRandomEngine(std::string seed_string)
    {
        return GetRandomEngines(1, seed_string)[0];
    }
}
}
