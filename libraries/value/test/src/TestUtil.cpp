////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TestUtil.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TestUtil.h"

#include <value/include/ComputeContext.h>
#include <value/include/Matrix.h>
#include <value/include/Tensor.h>
#include <value/include/Value.h>
#include <value/include/Vector.h>

#include <math/include/Matrix.h>
#include <math/include/Tensor.h>

#include <utilities/include/Boolean.h>
#include <utilities/include/MemoryLayout.h>

#include <testing/include/testing.h>

#include <iomanip>
#include <iostream>
#include <string>
#include <type_traits>

using namespace ell::utilities;
using namespace ell::value;
namespace math = ell::math;
using math::Dimension;
using math::MatrixLayout;

template <MatrixLayout layout>
using LayoutType = std::integral_constant<MatrixLayout, layout>;

namespace ell
{
void DebugPrint(std::string message)
{
    GetContext().DebugPrint(message);
}

void PrintMatrix(std::string indent, Matrix e)
{
    if (!e.GetValue().IsConstant())
    {
        std::cout << "cannot print non constant matrix";
        return;
    }
    if (e.Type() == ValueType::Undefined)
    {
        std::cout << "Undefined";
        return;
    }
    else if (e.Type() == ValueType::Void)
    {
        std::cout << "void";
        return;
    }
    std::cout << std::setprecision(10);
    int rows = static_cast<int>(e.Rows());
    int cols = static_cast<int>(e.Columns());
    for (int i = 0; i < rows; i++)
    {
        std::cout << indent;
        for (int j = 0; j < cols; j++)
        {
            if (j > 0)
            {
                std::cout << ", ";
            }
            Scalar s = e(i, j);
            switch (s.GetType())
            {
            case ValueType::Undefined:
                break;
            case ValueType::Void:
                break;
            case ValueType::Boolean:
                std::cout << (bool)s.Get<Boolean>();
                break;
            case ValueType::Char8:
                std::cout << s.Get<char>();
                break;
            case ValueType::Byte:
                std::cout << s.Get<uint8_t>();
                break;
            case ValueType::Int16:
                std::cout << s.Get<int16_t>();
                break;
            case ValueType::Int32:
                std::cout << s.Get<int32_t>();
                break;
            case ValueType::Int64:
                std::cout << s.Get<int64_t>();
                break;
            case ValueType::Float:
                std::cout << s.Get<float>();
                break;
            case ValueType::Double:
                std::cout << s.Get<double>();
                break;
            }
        }
        std::cout << "\n";
    }
}

Scalar EqualEpsilon(Scalar x, Scalar y, double epsilon)
{
    if (x.GetType() == ValueType::Int32)
    {
        return x == y;
    }
    Scalar e = Allocate(ValueType::Double, ScalarLayout);
    e = epsilon;
    Scalar tens = Floor(Log10(Cast(x, ValueType::Double)));
    If(tens > 0.0, [&] {
        // then we have some precision already on the left hand side of the decimal place, so remove that from epsilon
        e *= Pow(10.0, tens);
    });
    Scalar rx = Allocate(ValueType::Double, ScalarLayout);
    Scalar ry = Allocate(ValueType::Double, ScalarLayout);
    rx = Floor(Cast(x, ValueType::Double) / e) * e;
    ry = Floor(Cast(y, ValueType::Double) / e) * e;
    return rx == ry;
}

Scalar NotEqualEpsilon(Scalar x, Scalar y, double epsilon)
{
    if (x.GetType() == ValueType::Int32)
    {
        return x != y;
    }
    Scalar ep = epsilon;
    Scalar e = Allocate(ValueType::Double, ScalarLayout);
    e = epsilon;
    Scalar tens = Floor(Log10(Cast(x, ValueType::Double)));
    If(tens > 0.0, [&] {
        // then we have some precision already on the left hand side of the decimal place, so remove that from epsilon
        e *= Pow(10.0, tens);
    });
    Scalar rx = Allocate(ValueType::Double, ScalarLayout);
    Scalar ry = Allocate(ValueType::Double, ScalarLayout);
    rx = Floor(Cast(x, ValueType::Double) / e) * e;
    ry = Floor(Cast(y, ValueType::Double) / e) * e;
    Scalar diff = Abs(rx - ry);
    InvokeForContext<ComputeContext>([&] {
        double t = tens.Get<double>();
        double dx = diff.Get<double>();
        if (dx > epsilon)
        {
            std::cout << std::setprecision(10);
            std::cout << "  NotEqualEpsilon failed: t=" << t << ": " << dx << " >  " << epsilon << "\n";
        }
    });
    return diff > ep;
}

Scalar Verify(Vector actual, Vector expected, double epsilon)
{
    Scalar fail = 1;
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;
    For(actual, [&](Scalar index) {
        Scalar x = actual(index);
        Scalar y = expected(index);
        If(NotEqualEpsilon(x, y, epsilon), [&] {
            ok = fail;
        });
    });
    If(ok != 0, [&] {
        DebugPrint("## Vector compare failed\n");
        DebugPrint("  Expected: ");
        DebugPrintVector(expected);
        DebugPrint("\n");
        DebugPrint("  Actual:   ");
        DebugPrintVector(actual);
        DebugPrint("\n");
    });
    return ok;
}

Scalar VerifyDifferent(Vector actual, Vector expected, double epsilon)
{
    Scalar fail = 1;
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;
    For(actual, [&](Scalar index) {
        Scalar x = actual(index);
        Scalar y = expected(index);
        If(EqualEpsilon(x, y, epsilon), [&] {
            ok = fail;
        });
    });
    If(ok != 0, [&] {
        DebugPrint("## Vector are not different\n");
        DebugPrint("  Expected: ");
        DebugPrintVector(expected);
        DebugPrint("\n");
        DebugPrint("  Actual:   ");
        DebugPrintVector(actual);
        DebugPrint("\n");
    });
    return ok;
}

Scalar Verify(Matrix actual, Matrix expected, double epsilon)
{
    Scalar fail = 1;
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;
    For(actual, [&](Scalar row, Scalar col) {
        Scalar x = actual(row, col);
        Scalar y = expected(row, col);
        If(NotEqualEpsilon(x, y, epsilon), [&] {
            ok = fail;
        });
    });
    If(ok != 0, [&] {
        DebugPrint("## Matrix compare failed\n");
        InvokeForContext<ComputeContext>([&](auto&) {
            std::cout << "Expected: \n";
            PrintMatrix("   ", expected);
            std::cout << "\n";
            std::cout << "Actual:   \n";
            PrintMatrix("   ", actual);
            std::cout << "\n";
        });
    });
    return ok;
}

Scalar Verify(Tensor actual, Tensor expected, double epsilon)
{
    Scalar fail = Cast(1, ValueType::Int32);
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;
    For(actual, [&](Scalar row, Scalar col, Scalar ch) {
        Scalar x = actual(row, col, ch);
        Scalar y = expected(row, col, ch);
        If(NotEqualEpsilon(x, y, epsilon), [&] {
            DebugPrint("## Tensor compare failed\n");
            ok = fail;
        });
    });
    return ok;
}
} // namespace ell
