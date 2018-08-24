////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelEditor.cpp (model)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelEditor.h"

namespace ell
{
namespace model
{

void ModelEditor::ResetInputPort(const InputPortBase* port, const OutputPortBase& newInput)
{
    // Luckily nothing is ever really const in the codebase. If that changes, this is UB
    const_cast<InputPortBase*>(port)->SetInput(&newInput);
    port->GetNode()->RegisterDependencies();
}

}
}