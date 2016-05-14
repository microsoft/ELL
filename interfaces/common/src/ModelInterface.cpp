////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
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

    uint64_t Model::NumLayers() const
    {
        return _model->NumLayers(); 
    }
    
    uint64_t Model::GetRequiredLayerSize(uint64_t layerIndex) const 
    {
        return _model->GetRequiredLayerSize(layerIndex);
    }
    
    const layers::Layer& Model::GetLayer(uint64_t layerIndex) const 
    {
        return _model->GetLayer(layerIndex); 
    }
    
    void Model::Save(const std::string& filename) const
    {
        auto outputStream = utilities::OpenOfstream(filename);
        _model->Save(outputStream);
    }
    
    const layers::Model& Model::GetModel() const 
    { 
        return *_model; 
    }
    
    layers::Model& Model::GetModel() 
    { 
        return *_model; 
    }
}
