#pragma once

#include <string>
#include <vector>

///<summary>Execute the given python script, passing the given arguments as sys.argv </summary>
void ExecutePythonScript(const std::string& script, const std::vector<std::string>& args);

