//
// NodeInput
// 

#include "NodeInput.h"
#include "Node.h"

NodeOutputBase::OutputType NodeInput::GetType() const
{ 
    return _node->GetOutput(_outputIndex).GetType();
}
