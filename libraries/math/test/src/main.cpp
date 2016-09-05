////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Matrix.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <random>


void TestMatrix()
{
    math::DoubleColumnVector v(10);
    v.Fill(2);

    auto u = v.GetReference();
    u.Fill(3);

    auto w = v.GetConstReference();
    
    auto uu = u.GetSubVector(2, 2, 2);
    uu.Reset();
    uu[1] = 5;
    auto zz = uu.GetConstReference();

    auto ww = w.GetSubVector(2, 2);

    double x;
    math::TensorOperations::Product(ww.Transpose(), zz, x);
    auto y = math::TensorOperations::Dot(v, v);
}

/// Runs all tests
///
int main()
{
    TestMatrix();

    if(testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
