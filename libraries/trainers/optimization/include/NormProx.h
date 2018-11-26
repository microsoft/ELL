////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NormProx.h (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <math/include/Vector.h>

#include <vector>

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        /// <summary> Inplace calculation of the L1 weighted proximal operator </summary>
        ///
        /// <param name="v"> The input and output of the operator. </param>
        /// <param name="beta"> The weight of the L1 norm in the proximal operator definition. </param>
        void L1Prox(math::ColumnVectorReference<double> v, double beta = 1.0);

        /// <summary> Inplace calculation of the L-infinity weighted proximal operator </summary>
        ///
        /// <param name="v"> The input and output of the operator. </param>
        /// <param name="scratch"> A vector of scratch space. </param>
        /// <param name="beta"> The weight of the L-infinity norm in the proximal operator definition. </param>
        void LInfinityProx(math::ColumnVectorReference<double> v, std::vector<size_t>& scratch, double beta = 1.0);
    } // namespace optimization
} // namespace trainers
} // namespace ell
