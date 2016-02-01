// CompilableShift.cpp

#include "CompilableShift.h"

void CompilableShift::BackwardPass(uint64 currentLayerIndex, vector<vector<vector<AddToAction>>>& actions) const
{
    for(uint64 column = 0; column < Size(); ++column)
    {
        auto coordinate = _coordinates[column];
        auto& inputActionList = actions[coordinate.GetRow()][coordinate.GetColumn()];
        const auto& outputActionList = actions[currentLayerIndex][column];

        //create the linear operation
        LinearOperation inputOperation(1.0, _values[column]);

        for(const auto& action : outputActionList)
        {
            const LinearOperation& outputOperation = action.GetOperation();
            const string& targetVariableName = action.GetTargetVariableName();
            inputActionList.emplace_back(outputOperation.Compound(inputOperation), targetVariableName);
        }
    }
}
