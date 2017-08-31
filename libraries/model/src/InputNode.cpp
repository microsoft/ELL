////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     InputNode.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InputNodeBase.h"

namespace ell
{
namespace model
{
    InputNodeBase::InputNodeBase(OutputPortBase& output, math::TensorShape shape)
        : CompilableNode({}, { &output }), _outputBase(output), _shape(shape)
    {
    }

    ell::utilities::ArchiveVersion InputNodeBase::GetArchiveVersion() const
    {
        return ell::utilities::ArchiveVersion{ 2 };
    }
}
}
