////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IIRFilterValue_test.cpp (emittable_functions)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IIRFilter_test.h"
#include "Emittable_functions_util.h"

#include <emitters/include/IRModuleEmitter.h>

#include <emittable_functions/include/IIRFilter.h>

#include <testing/include/testing.h>

#include <value/include/ComputeContext.h>
#include <value/include/LLVMContext.h>
#include <value/include/Matrix.h>
#include <value/include/Tensor.h>
#include <value/include/Value.h>
#include <value/include/Vector.h>

using namespace ell::emitters;
using namespace ell::utilities;
using namespace ell::value;
using namespace ell::emittable_functions;

namespace
{

template <typename T>
constexpr ValueType GetValueType(const std::vector<T>& value)
{
    return ell::value::GetValueType<typename std::decay_t<decltype(value)>::value_type>();
}

} // namespace

namespace ell
{

void TestIIRFilter()
{
    // Simple pre-emph filter
    std::vector<double> signal{ 1.0, 0.0, 0.0, 0.0 };
    std::vector<double> b{ 1.0 };
    std::vector<double> a{ -0.95 };
    std::vector<double> expected{ 1.0, 0.95, 0.95 * 0.95, 0.95 * 0.95 * 0.95 };
    TestIIRFilter(signal, b, a, expected);
}

void TestIIRFilter(std::vector<double> signal, std::vector<double> b, std::vector<double> a, std::vector<double> expected)
{
    auto valueType = GetValueType(signal);
    int resultSize = (int)signal.size();
    int signalSize = (int)signal.size();
    int aSize = (int)a.size();
    int bSize = (int)b.size();
    auto filter1D = DeclareFunction("TestIIRFilter")
                        .Returns(Value{ valueType, MemoryLayout({ resultSize }) })
                        .Parameters(Value{ valueType, MemoryLayout({ signalSize }) },
                                    Value{ valueType, MemoryLayout({ bSize }) },
                                    Value{ valueType, MemoryLayout({ aSize }) })
                        .Define([](Vector signal, Vector b, Vector a) {
                            return FilterSamples(signal, { b, a });
                        });

    InvokeForContext<ComputeContext>([&](auto&) {
        bool ok = true;
        Vector result = filter1D(signal, b, a);
        For(result, [&](Scalar index) {
            auto indexInt = index.Get<int>();
            ok &= testing::IsEqual(expected[indexInt], result[index].Get<double>());
        });
        testing::ProcessTest("Testing 1D IIR filter with Vector", ok);
    });

    InvokeForContext<TestLLVMContext>(PrintIR);
}

} // namespace ell
