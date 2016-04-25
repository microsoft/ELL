////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     LoadStack.h (common)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "StackLoadArguments.h"

//layers
#include "Stack.h"

namespace common
{
    /// <summary> Loads a stack from a file, or creates a new one if given an empty filename. </summary>
    ///
    /// <param name="stackLoadArguments"> Stack load arguments. </param>
    ///
    /// <returns> A unique pointer to the stack. </returns>
    layers::Stack LoadStack(const StackLoadArguments& stackLoadArguments);
}