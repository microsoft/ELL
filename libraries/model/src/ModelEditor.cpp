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

void ModelEditor::ResetInputPort(const InputPortBase* port, PortElementsBase portElements)
{
    // Luckily nothing is ever really const in the codebase. If that changes, this is UB
    assert(port->GetType() == portElements.GetPortType());
    const_cast<PortElementsBase&>(port->_inputElements) = std::move(portElements);
    const_cast<InputPortBase*>(port)->ComputeParents();
    port->GetNode()->RegisterDependencies();
}

}
}