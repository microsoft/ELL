////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelMaker.cpp (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelMaker.h"

using namespace ell;

ModelMaker::ModelMaker()
{
}

ModelMaker::ModelMaker(const ModelMaker& src)
    : _model(src._model)
{
}

ModelMaker::ModelMaker(ModelMaker&& src)
    : _model(std::move(src._model))
{
}
