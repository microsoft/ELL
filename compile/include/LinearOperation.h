////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     LinearOperation.h (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <iostream>
#include <string>

/// <summary> A linear operation. </summary>
class LinearOperation
{
public:

    /// <summary> Constructs an instance of LinearOperation. </summary>
    ///
    /// <param name="a"> The multiplicative coefficent. </param>
    /// <param name="b"> The additive coefficient. </param>
    LinearOperation(double a=1.0, double b=0.0);

    /// <summary> Compounds this linear operation with an input operation. </summary>
    ///
    /// <param name="inputOperation"> The input operation. </param>
    ///
    /// <returns> The compound LinearOperation. </returns>
    LinearOperation Compound(const LinearOperation& inputOperation) const;

    /// <summary> Query if this operation is a null operation (a null operation does not change its target). </summary>
    ///
    /// <returns> true if the action is a null action. </returns>
    bool IsNull() const;

    /// <summary> Prints the operation. </summary>
    ///
    /// <param name="sourceVariableName"> Name of the source variable to which this operation is applied. </param>
    /// <param name="os"> [in,out] Output stream to write data to. </param>
    void Print(const std::string& sourceVariableName, std::ostream& os) const;

private:
    double _a = 0;
    double _b = 0;
};

