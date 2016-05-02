////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Predictiors.i (interfaces)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%module predictors

%include typemaps.i

%{
#define SWIG_FILE_WITH_INIT
#include "LinearPredictor.h"
#include "MutableDecisionTree.h"
#include "Coordinatewise.h"
#include "Sum.h"
#include "Model.h"
%}

%include "LinearPredictor.h"
// %include "MutableDecisionTree.h"  # currently doesn't compile

%extend predictors::LinearPredictor
{
    void AddToModel(interfaces::Model& model, const layers::CoordinateList& inputCoordinates) const
    {
        uint64_t layerIndex = model.GetModel().AddLayer(std::make_unique<layers::Coordinatewise>($self->GetVector(), inputCoordinates, layers::Coordinatewise::OperationType::multiply));
        auto coordinates = model.GetModel().BuildCoordinateList(layerIndex);
        layerIndex = model.GetModel().AddLayer(std::make_unique<layers::Sum>(coordinates));
        layerIndex = model.GetModel().AddLayer(std::make_unique<layers::Coordinatewise>($self->GetBias(), layers::Coordinate{ layerIndex, 0 }, layers::Coordinatewise::OperationType::add));
    }
}
