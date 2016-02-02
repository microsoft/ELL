// random_engines.h

#pragma once

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <random>
using std::default_random_engine;
using std::random_device;
using std::seed_seq;

namespace utilities 
{
    vector<default_random_engine> GetRandomEngines(int num = 1, string seed_string = "");

    default_random_engine GetRandomEngine(string seed_string = "");
}