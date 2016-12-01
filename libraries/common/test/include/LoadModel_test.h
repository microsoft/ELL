#pragma once
//
// LoadModel tests
//

#include <string>

namespace emll
{
void TestLoadSampleModels();
void TestLoadTreeModels();
void TestLoadSavedModels();
void TestSaveModels(std::string ext);
}