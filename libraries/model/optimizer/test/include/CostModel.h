////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CostModel.h (model/optimizer)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Cost.h"
#include "Environment.h"

#include <model/include/Submodel.h>

#include <memory>

namespace ell
{
namespace model
{
    namespace optimizer
    {
        /// <summary> An oracle that returns the cost of evaluating a submodel in a given environment. </summary>
        class CostModel
        {
        public:
            virtual ~CostModel() = default;

            /// <summary> Returns `true` if the oracle can provide a cost for evaluating the given submodel and environment. </summary>
            virtual bool HasCost(const Submodel& submodel, const Environment& environment) const = 0;

            /// <summary> Returns the cost for evaluating given submodel and environment. </summary>
            virtual Cost GetCost(const Submodel& submodel, const Environment& environment) const = 0;
        };
    } // namespace optimizer
} // namespace model
} // namespace ell
