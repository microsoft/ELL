// CompilableLayer.cpp

#include "CompilableLayer.h"

// initialize global variable counter
uint64 CompilableLayer::_tempVariableCounter = 0;

uint64 CompilableLayer::NumTempVariableNames()
{
    return _tempVariableCounter;
}

string CompilableLayer::GetNextTempVariableName()
{
    string name = "var" + _tempVariableCounter;
    ++_tempVariableCounter;
    return name;
}
