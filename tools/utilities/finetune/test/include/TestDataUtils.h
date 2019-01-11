////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TestDataUtils.h (finetune_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// Main driver function
void TestDataUtils();

// Individual tests
void TestLoadBinaryLabelDataContainer();
void TestLoadMultiClassDataContainer();
void TestLoadCifarDataContainer();
void TestLoadMnistDataContainer();

void TestCreateVectorLabelDataContainer();
void TestBinaryGetDatasetInputs();
void TestMultiClassGetDatasetInputs();
void TestVectorLabelGetDatasetInputs();
void TestGetDatasetOutputs();
void TestGetImageMatrixDataset();
void TestGetUnrolledImageDataset();
void TestBinaryGetModelAccuracy();
void TestMultiClassGetModelAccuracy();
