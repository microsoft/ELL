////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelBuilder.cpp (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelBuilder.h"

namespace ell
{
ModelBuilder::ModelBuilder()
{
}

ModelBuilder::ModelBuilder(const ModelBuilder& src)
    : _model(src._model)
{
}

ModelBuilder::ModelBuilder(ModelBuilder&& src)
    : _model(std::move(src._model))
{
}
}