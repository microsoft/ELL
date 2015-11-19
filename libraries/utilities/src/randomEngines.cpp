// random_engines.cpp

#include "randomEngines.h"

using std::move;

namespace utilities
{
    vector<default_random_engine> GetRandomEngines(int num, string seed_string)
    {
        vector<int> seeds(num);

        if(seed_string != "")
        {
            seed_seq seq(seed_string.begin(), seed_string.end());
            seq.generate(seeds.begin(), seeds.end());
        }
        else
        {
            random_device d;
            seed_seq seq{d(),d(),d(),d(),d(),d(),d(),d()};
            seq.generate(seeds.begin(), seeds.end());
        }

        // create the independent generators
        vector<default_random_engine> rngs;
        rngs.reserve(num);
        for(auto seed : seeds)
        {
            rngs.emplace_back(seed);
        }

        return rngs;
    }

    default_random_engine GetRandomEngine(string seed_string)
    {
        return GetRandomEngines(1, seed_string)[0];
    }
}