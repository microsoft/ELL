// CompilableLayer.cpp

#include "CompilableLayer.h"

// initialize global variable counter
uint64 CompilableLayer::_tempVariableCounter = 0;

uint64 CompilableLayer::NumTempVariableNames()
{
    return _tempVariableCounter;
}

std::string CompilableLayer::GetNextTempVariableName()
{
    std::string name = "var" + std::to_string(_tempVariableCounter);
    ++_tempVariableCounter;
    return name;
}
