////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PrintModel.h (print)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/Model.h>

#include <ostream>

namespace ell
{
struct PrintModelOptions
{
    bool includeNodeId = false;
    bool nodeDetails = true;
};
void PrintModel(const model::Model& model, std::ostream& out, const PrintModelOptions& options);
} // namespace ell
