////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilableIRFunction.tcc (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace emitters
{
    template <typename ValueType>
    ValueType IRAddFunction<ValueType>::Compute(ValueType x, ValueType y) const
    {
        return x + y;
    }

    template <typename ValueType>
    LLVMValue IRAddFunction<ValueType>::Compile(IRFunctionEmitter& function, LLVMValue x, LLVMValue y) const
    {
        LLVMValue sum = function.Operator(emitters::GetAddForValueType<ValueType>(), x, y);
        return sum;
    }
}
}
