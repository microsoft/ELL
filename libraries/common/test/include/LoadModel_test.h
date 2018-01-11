#pragma once
//
// LoadModel tests
//

#include <string>

namespace ell
{
void TestLoadSampleModels();
void TestLoadTreeModels();
void TestLoadSavedModels(const std::string& examplePath);
void TestSaveModels();
}