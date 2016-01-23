// CompilableLayer.cpp

#include "CompilableLayer.h"

// initialize global variable counter
uint64 CompilableLayer::_tempVariableCounter = 0;

void CompilableLayer::AddAction(uint64 index, const AddToAction & action)
{
    if(_actions.size() < index+1)
    {
        _actions.resize(index+1);
    }
    _actions[index].push_back(action);
}

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
