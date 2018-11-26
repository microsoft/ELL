////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelMaker.cpp (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelMaker.h"

#include <model/include/ModelTransformer.h>

using namespace ell;

ModelMaker::ModelMaker()
{
}

ModelMaker::ModelMaker(const ModelMaker& src)
{
    model::ModelTransformer transformer;
    model::TransformContext context;
    _model = transformer.CopyModel(src._model, context);
}

ModelMaker::ModelMaker(ModelMaker&& src) :
    _model(std::move(src._model))
{
}
