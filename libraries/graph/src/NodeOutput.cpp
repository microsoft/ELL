//
// NodeOutput
// 

#include "NodeOutput.h"

template<>
NodeOutputBase::OutputType NodeOutputBase::GetTypeCode<double>() 
{ 
    return NodeOutputBase::OutputType::Real; 
}

template<>
NodeOutputBase::OutputType NodeOutputBase::GetTypeCode<int>() 
{ 
    return NodeOutputBase::OutputType::Integer; 
}

template<>
NodeOutputBase::OutputType NodeOutputBase::GetTypeCode<bool>() 
{ 
    return NodeOutputBase::OutputType::Boolean; 
}
