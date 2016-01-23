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
    /// Adds an action to the layer
    ///
    void AddAction(uint64 index, const AddToAction& action);

    /// This function is called for each layer, from the bottom up
    ///
    virtual void BackwardPass() = 0;

    //virtual void ProcessForward() = 0;

    /// Prints the code that executes the actions required by this layer
    ///
    //virtual void Print() const = 0;

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

 