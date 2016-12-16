////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     EvaluatorArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "EvaluatorArguments.h"

namespace ell
{
namespace common
{
    void ParsedEvaluatorArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            evaluationFrequency,
            "evaluationFrequency",
            "ef",
            "Frequency of evaluation, a value of k>0 means evaluate every k iterations",
            1);

        parser.AddOption(
            addZeroEvaluation,
            "addZeroEvaluation",
            "aze",
            "Add an evaluation using the constant zero predictor",
            true);
    }
}
}
