// random_engines.h

#pragma once

// stl
#include <string>
#include <vector>
#include <random>

namespace utilities 
{
    std::vector<std::default_random_engine> GetRandomEngines(int num = 1, std::string seed_string = "");

    std::default_random_engine GetRandomEngine(std::string seed_string = "");
}
