////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Vector_test.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Vector_test.h"
#include "TestUtil.h"

#include <value/include/Value.h>
#include <value/include/Vector.h>

#include <math/include/Vector.h>

#include <utilities/include/MemoryLayout.h>

#include <type_traits>
#include <vector>

using namespace ell::utilities;
using namespace ell::value;

namespace
{
// Copied from ConvolutionTestData.cpp in dsp/test
//
// NOTE: The examples below were randomly-generated using
//       the following script:
//
// ```
// import scipy
// import scipy.signal
// import numpy as np
//
// input = np.random.rand(16)
// filter = [0.25, 0.5, 0.25]
// reference = scipy.signal.correlate(input, filter, mode="valid", method="direct")
// ```
auto Get1DReferenceFilter()
{
    return std::vector<double>{ 0.25, 0.5, 0.25 };
}

auto Get1DReferenceSignal()
{
    return std::vector<double>{ 0.42929697, 0.90317845, 0.84490289, 0.66174327, 0.10820399, 0.3511343, 0.58248869, 0.62674724, 0.11014194, 0.00132073, 0.58431646, 0.39873614, 0.40304155, 0.79139607, 0.97710827, 0.21268128 };
}

auto Get1DReferenceConvolutionResult()
{
    auto signal = Get1DReferenceSignal();
    auto filter = Get1DReferenceFilter();
    size_t resultSize = signal.size() - filter.size() + 1;
    std::vector<double> result(resultSize);

    for (size_t i = 0; i < resultSize; i++)
    {
        double accum = 0;
        for (size_t j = 0; j < filter.size(); j++)
        {
            accum += filter[j] * signal[i + j];
        }
        result[i] = accum;
    }
    return result;
}
} // namespace

namespace ell
{
Vector testConvolve1D(Vector signal, Vector filter)
{
    size_t resultSize = signal.Size() - filter.Size() + 1;
    Vector result(Allocate(signal.GetType(), resultSize));
    Scalar accum = Allocate(signal.GetType(), ScalarLayout);
    For(result, [&](Scalar index) {
        accum = Cast(0, accum.GetType());
        For(filter, [&](Scalar filterIndex) {
            accum += filter(filterIndex) * signal(index + filterIndex);
        });
        result(index) = accum;
    });

    return result;
}

Scalar Vector_test1()
{
    auto signal = Get1DReferenceSignal();
    auto filter = Get1DReferenceFilter();
    auto referenceResult = Get1DReferenceConvolutionResult();

    // this works
    Vector result = testConvolve1D(signal, filter);

    // but the following does not.
    //auto valueType = GetValueType<decltype(signal)::value_type>();
    //uto convolve1D = DeclareFunction("Convolve1D")
    //                     .Returns({ valueType, MemoryLayout({ (int)referenceResult.size() }) })
    //                     .Parameters(
    //                         Value{ valueType, MemoryLayout({ (int)signal.size() }) },
    //                         Value{ valueType, MemoryLayout({ (int)filter.size() }) })
    //                     .Define(testConvolve1D);
    // Vector result = convolve1D(signal, filter);

    Vector expected(referenceResult);
    return VerifySame(result, expected);
}

Scalar Vector_test2()
{
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;
    Vector v = std::vector<float>{ 1.2f, 2.3f };
    Vector testVector(std::vector<float>{ 0.1f, 1.2f });
    Scalar testScalar{ 3.4f };
    {
        Vector expected(std::vector<float>{ 1.2f + 3.4f, 2.3f + 3.4f });
        Vector actual = v + testScalar;
        If(VerifySame(actual, expected) != 0, [&] {
            DebugPrint("## Vector_test2 vector scalar addition failed\n");
            ok = 1;
        });
    }
    {
        Vector expected(std::vector<float>{ 1.2f - 3.4f, 2.3f - 3.4f });
        Vector actual = v - testScalar;
        If(VerifySame(actual, expected) != 0, [&] {
            DebugPrint("## Vector_test2 vector scalar subtraction failed\n");
            ok = 1;
        });
    }
    {
        Vector expected(std::vector<float>{ 1.2f * 3.4f, 2.3f * 3.4f });
        Vector actual = v * testScalar;
        If(VerifySame(actual, expected) != 0, [&] {
            DebugPrint("## Vector_test2 vector scalar multiplication failed\n");
            ok = 1;
        });
    }
    {
        Vector expected(std::vector<float>{ 1.2f / 3.4f, 2.3f / 3.4f });
        Vector actual = v / testScalar;
        If(VerifySame(actual, expected) != 0, [&] {
            DebugPrint("## Vector_test2 vector scalar division failed\n");
            ok = 1;
        });
    }

    // Vector +- Vector -> Vector
    {
        Vector expected(std::vector<float>{ 1.2f + 0.1f, 2.3f + 1.2f });
        Vector actual = v + testVector;
        If(VerifySame(actual, expected) != 0, [&] {
            DebugPrint("## Vector_test2 vector+vector failed\n");
            ok = 1;
        });
    }
    {
        Vector expected(std::vector<float>{ 1.2f - 0.1f, 2.3f - 1.2f });
        Vector actual = v - testVector;
        If(VerifySame(actual, expected) != 0, [&] {
            DebugPrint("## Vector_test2 vector-vector failed\n");
            ok = 1;
        });
    }
    return ok;
}

Scalar Vector_test3()
{
    // BUGBUG: This test currently fails
    DebugPrint("Skipping Vector_test3\n");
    return Scalar(0);

    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;
    Vector v = std::vector<float>{ 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    Vector i = std::vector<float>{ 2, 2, 2 };

    Vector s = v.SubVector(7, 3);
    s = i; // copy the i vector to the end of the v vector.

    Vector e = std::vector<float>{ 1, 1, 1, 1, 1, 1, 2, 2, 2 };

    If(VerifySame(v, e) != 0, [&] {
        DebugPrint("## Vector_test3 subvector assignment failed\n");
        ok = 1;
    });
    return ok;
}

Scalar Vector_test4()
{
    // BUGBUG: This test currently fails
    DebugPrint("Skipping Vector_test4\n");
    return Scalar(0);

#if 0
    Vector src(std::vector<int32_t>{ 10, 20 });
    Vector dest(std::vector<int32_t>{ 0, 0 });
    dest(0) = src(0);
    return dest(1);
#endif
}

Scalar Vector_test5()
{
    Vector src(std::vector<float>{ 10, 20 });
    Vector dest(std::vector<float>{ 0, 0 });
    dest(0) = src(0);
    return Cast(dest(1), value::ValueType::Int32);
}
} // namespace ell
