////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoadTestModels.h (finetune_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/Model.h>

#include <string>

std::string GetFullyConnectedMNISTModelPath();
std::string GetConvolutionalMNISTModelPath();
std::string GetSuperSimpleConvolutionalMNISTModelPath();
std::string GetTrivialConvolutionalModelPath();
std::string GetTrivialConvolutionalModel2Path();

ell::model::Model LoadFullyConnectedMNISTModel();
ell::model::Model LoadConvolutionalMNISTModel();
ell::model::Model LoadSuperSimpleConvolutionalMNISTModel();
ell::model::Model LoadTrivialConvolutionalModel();
ell::model::Model LoadTrivialConvolutionalModel2();

ell::model::Model GetNodeFindingTestModel();
