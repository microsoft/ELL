////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ParameterGenerator.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

namespace common
{
    template <typename ParametersType>
    class ParametersGenerator
    {
    public:
        std::vector<ParameterType> GetParameterVector() const; 

    };

}

#include "../tcc/ParameterGenerator.tcc"
