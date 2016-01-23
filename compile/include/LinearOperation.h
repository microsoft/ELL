// LinearOperation.h

#pragma once

// stl
#include <iostream>
using std::ostream;

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
    template<typename PrintableInputType>
    void Print(ostream& os, const PrintableInputType& printableInput) const;

private:
    double _a = 0;
    double _b = 0;
};

#include "../tcc/LinearOperation.tcc"
