// CompilableLayer.h

#pragma once

#include "AddToAction.h"

// types
#include "types.h"

// stl
#include <vector>
using std::vector;

#include <string>
using std::string;

/// An interface for classes that can print human-friendly descriptions of themselves
///
class CompilableLayer 
{
public:
    /// This function is called for each layer, from the bottom up
    ///
    virtual void BackwardPass(uint64 currentLayerIndex, vector<vector<vector<AddToAction>>>& actions) const = 0;

    //virtual void ProcessForward() = 0;

    /// \returns The number of temp variable names allocated
    ///
    static uint64 NumTempVariableNames();

protected:
    /// \returns The next available variable name
    ///
    static string GetNextTempVariableName(); 

private:
    static uint64 _tempVariableCounter;
    vector<vector<AddToAction>> _actions;
};

 