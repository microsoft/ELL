////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     ModelInterface.cpp (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelInterface.h"

// utilities
#include "Files.h"

#include <ostream>
#include <string>
namespace interfaces
{
    Model::Model() : _model(std::make_shared<layers::Model>()) 
    {
    }

    Model::Model(layers::Model&& model) : _model(std::make_shared<layers::Model>(std::move(model))) 
    {
    }

    void Model::Save(const std::string& filename) const
    {
        auto outputStream = utilities::OpenOfstream(filename);
        _model->Save(outputStream);
    }
}