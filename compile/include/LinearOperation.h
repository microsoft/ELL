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

class LinearOperation
{
public:
    ///
    ///
    LinearOperation(double a=1.0, double b=0.0);

    ///
    ///
    LinearOperation Compound(const LinearOperation& inputOperation) const;

    /// \returns True if the operation is not the null operations, namely, if a != 0 or b != 0
    ///
    bool IsNull() const;

    ///
    ///
    void Print(std::string sourceVariableName, std::ostream& os) const;

private:
    double _a = 0;
    double _b = 0;
};

