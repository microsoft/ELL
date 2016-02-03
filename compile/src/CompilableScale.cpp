// CompilableScale.cpp

#include "CompilableScale.h"

void CompilableScale::BackwardPass(uint64 currentLayerIndex, vector<vector<vector<AddToAction>>>& actions) const // TODO move this function to common coordinatewise class
{
    for(uint64 column = 0; column < Size(); ++column)
    {
        auto coordinate = _coordinates[column];
        auto& inputActionList = actions[coordinate.GetRow()][coordinate.GetColumn()];
        const auto& outputActionList = actions[currentLayerIndex][column];

        //create the linear operation
        LinearOperation inputOperation(_values[column], 0.0);

        for(const auto& action : outputActionList)
        {
            const LinearOperation& outputOperation = action.GetOperation();
            const std::string& targetVariableName = action.GetTargetVariableName();
            inputActionList.emplace_back(outputOperation.Compound(inputOperation), targetVariableName);
        }
    }
}
