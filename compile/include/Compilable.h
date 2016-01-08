// Compilable.h

#pragma once

#include "Action.h"

#include "types.h"

#include <vector>
using std::vector;

/// An interface for classes that can print human-friendly descriptions of themselves
///
class Compilable 
{
public:
    /// Returns the number of elements in the layer
    ///
    virtual uint64 Size() const = 0;

private:
    vector<vector<Action>> _actions;
};

 