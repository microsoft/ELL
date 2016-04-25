////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     LoadStack.cpp (common)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoadStack.h"

// utilities
#include "Files.h"

namespace common
{
    layers::Stack LoadStack(const StackLoadArguments& stackLoadArguments)
    {
        if (stackLoadArguments.inputStackFile != "")
        {
            auto inputMapFStream = utilities::OpenIfstream(stackLoadArguments.inputStackFile);
            utilities::XMLDeserializer deserializer(inputMapFStream);

            layers::Stack stack;
            deserializer.Deserialize(stack);
            return stack;
        }
        else
        {
            return layers::Stack();
        }
    }
}