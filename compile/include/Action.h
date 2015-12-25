// Action.h 

#pragma once

#include "types.h"

#include <string>
using std::string;

/// Encodes the operation: "variable_targetID += a*x + b"
///
struct Action
{
    string targetName = "";
    double a = 0;
    double b = 0;
};