/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NodesTestData.h (nodes_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <vector>

template <typename ValueType>
std::vector<ValueType> GetConvolutionTestData();

template <typename ValueType>
std::vector<ValueType> GetConvolutionTestFilter();

template <typename ValueType>
std::vector<ValueType> GetConvolutionTestResultSame();

template <typename ValueType>
std::vector<ValueType> GetCorrelationTestResultSame();
