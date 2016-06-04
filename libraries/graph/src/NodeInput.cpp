//
// NodeInput
// 

#include "NodeInput.h"
#include "Node.h"

#include <cassert>

NodeEdge::OutputType NodeInput::GetType() const
{ 
    assert(_type == _node->GetOutput(_outputIndex).GetType());
    return _type;
}
