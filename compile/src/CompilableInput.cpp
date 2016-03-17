////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CompilableInput.cpp (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableInput.h"

// stl
#include <stdexcept>

uint64_t CompilableInput::Size() const
{
    return Input::Size();
}

void CompilableInput::operator=(const layers::Input& input)
{
    Input::operator=(input);
}

void CompilableInput::operator=(const layers::Layer& layer)
{
    operator=(dynamic_cast<const layers::Input&>(layer));
}

void CompilableInput::SetActions(uint64_t currentLayerIndex, DataFlowGraph& graph) const 
{
    throw std::runtime_error("this place in the code should bever be reached");
}