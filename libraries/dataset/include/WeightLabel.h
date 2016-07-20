////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     WeightLabel.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <ostream>

namespace dataset
{
    class WeightLabel
    {
    public:

        /// <summary> Constructs an instance of WeightLabel. </summary>
        ///
        /// <param name="weight"> The weight. </param>
        /// <param name="label"> The label. </param>
        WeightLabel(double weight, double label);

        WeightLabel(const WeightLabel&) = default;

        /// <summary> Prints the weight label pair. </summary>
        ///
        /// <param name="os"> [in,out] The output stream. </param>
        void Print(std::ostream& os) const;

        double weight;
        double label;
    };
}