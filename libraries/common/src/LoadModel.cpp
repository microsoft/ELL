////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     LoadModel.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoadModel.h"

// utilities
#include "Files.h"

namespace common
{
    layers::Model LoadModel(const ModelLoadArguments& modelLoadArguments)
    {
        if (modelLoadArguments.inputModelFile != "")
        {
            auto inputMapFStream = utilities::OpenIfstream(modelLoadArguments.inputModelFile);
            utilities::XMLDeserializer deserializer(inputMapFStream);

            layers::Model model;
            deserializer.Deserialize(model);
            return model;
        }
        else
        {
            return layers::Model();
        }
    }
}
