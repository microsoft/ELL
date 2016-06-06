//
// Port
// 

#include "Port.h"
//
// Specializations of GetTypeCode
//
template<>
Port::OutputType Port::GetTypeCode<double>() 
{ 
    return Port::OutputType::Real; 
}

template<>
Port::OutputType Port::GetTypeCode<int>() 
{ 
    return Port::OutputType::Integer; 
}

template<>
Port::OutputType Port::GetTypeCode<bool>() 
{ 
    return Port::OutputType::Boolean; 
}
