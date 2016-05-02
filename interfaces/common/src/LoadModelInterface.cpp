////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LoadModelInterface.cpp (interfaces)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadModelInterface.h"

// common
#include "ModelLoadArguments.h"
#include "LoadModel.h"

// layers
#include "Model.h"

// interface
#include "ModelInterface.h"

namespace interfaces
{
    interfaces::Model LoadModel(const common::ModelLoadArguments& modelLoadArguments)
    {
        auto model = common::LoadModel(modelLoadArguments);
        return interfaces::Model(std::move(model));
    }

    interfaces::Model LoadModel(const std::string& filename)
    {
        common::ModelLoadArguments args;
        args.inputModelFile = filename;
        return interfaces::LoadModel(args);
    }
}
