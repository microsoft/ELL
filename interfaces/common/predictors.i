////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Predictiors.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%module predictors

%include typemaps.i

%{
#define SWIG_FILE_WITH_INIT
#include "LinearPredictor.h"
#include "DecisionTreePredictor.h"
#include "Coordinatewise.h"
#include "Sum.h"
#include "Model.h"
%}

%include "LinearPredictor.h"

%extend predictors::LinearPredictor
{
    void AddToModel(interfaces::Model& model, const layers::CoordinateList& inputCoordinates) const
    {
        auto weightsLayerCoordinates = model.GetModel().AddLayer(std::make_unique<layers::Coordinatewise>(std::vector<double>($self->GetVector()), inputCoordinates, layers::Coordinatewise::OperationType::multiply));
        auto sumLayerCoordinates = model.GetModel().AddLayer(std::make_unique<layers::Sum>(weightsLayerCoordinates));
        model.GetModel().AddLayer(std::make_unique<layers::Coordinatewise>($self->GetBias(), sumLayerCoordinates[0], layers::Coordinatewise::OperationType::add));
    }
}
