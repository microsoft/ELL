//
// NodeEdge
// 

#include "NodeEdge.h"
//
// Specializations of GetTypeCode
//
template<>
NodeEdge::OutputType NodeEdge::GetTypeCode<double>() 
{ 
    return NodeEdge::OutputType::Real; 
}

template<>
NodeEdge::OutputType NodeEdge::GetTypeCode<int>() 
{ 
    return NodeEdge::OutputType::Integer; 
}

template<>
NodeEdge::OutputType NodeEdge::GetTypeCode<bool>() 
{ 
    return NodeEdge::OutputType::Boolean; 
}
