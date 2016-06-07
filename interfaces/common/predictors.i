////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Predictiors.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%ignore predictors::LinearPredictor::AddToModel(layers::Model&, const layers::CoordinateList& ) const;
%ignore predictors::LinearPredictor::GetVector() const;
%ignore predictors::LinearPredictor::GetBias() const;
//%ignore predictors::DecisionTreePredictor;
%ignore predictors::DecisionTreePredictor::InteriorNode;

%{
#define SWIG_FILE_WITH_INIT
#include "LinearPredictor.h"
#include "DecisionTreePredictor.h"
#include "Coordinatewise.h"
#include "Sum.h"
#include "Model.h"
%}

//%import "Model.h"
//%import "ModelInterface.h"

#ifndef SWIGXML
%shared_ptr(predictors::LinearPredictor) 
#endif

%include "LinearPredictor.h"
%include "DecisionTreePredictor.h"

%extend predictors::LinearPredictor
{
    void AddToModel(interfaces::Model& model, const layers::CoordinateList& inputCoordinates) const
    {
        auto weightsLayerCoordinates = model.GetModel().AddLayer(std::make_unique<layers::Coordinatewise>(std::vector<double>($self->GetVector()), inputCoordinates, layers::Coordinatewise::OperationType::multiply));
        auto sumLayerCoordinates = model.GetModel().AddLayer(std::make_unique<layers::Sum>(weightsLayerCoordinates));
        model.GetModel().AddLayer(std::make_unique<layers::Coordinatewise>($self->GetBias(), sumLayerCoordinates[0], layers::Coordinatewise::OperationType::add));
    }
}
