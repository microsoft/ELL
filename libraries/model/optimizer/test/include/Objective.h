////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Objective.h (model/optimizer)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Cost.h"

namespace ell
{
namespace model
{
    namespace optimizer
    {
        /// <summary> Represents a real-valued function on the output of a CostModel. </summary>
        class Objective
        {
        public:
            using ObjectiveValue = double;

            virtual ~Objective() = default;

            /// <summary> Returns the value of the objective for the given cost. </summary>
            virtual ObjectiveValue Evaluate(const Cost& cost) const = 0;
        };
    } // namespace optimizer
} // namespace model
} // namespace ell
