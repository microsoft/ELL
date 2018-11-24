////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IREmitter.tcc (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace emitters
{
    template <typename OutputType>
    LLVMValue IREmitter::CastValue(LLVMValue pValue)
    {
        auto outputType = GetVariableType<OutputType>();
        return CastValue(pValue, outputType);
    }

    template <typename OutputType>
    LLVMValue IREmitter::CastUnsignedValue(LLVMValue pValue)
    {
        auto outputType = GetVariableType<OutputType>();
        return CastUnsignedValue(pValue, outputType);
    }

    template <typename ValueType>
    llvm::Constant* IREmitter::Pointer(ValueType* ptr)
    {
        auto ptrValue = Literal(reinterpret_cast<int64_t>(ptr));
        auto ptrType = PointerType(GetVariableType<ValueType>());
        return llvm::ConstantExpr::getIntToPtr(ptrValue, ptrType);
    }

    template <typename ListType>
    void IREmitter::BindArgumentNames(LLVMFunction pFunction, const ListType& arguments)
    {
        size_t i = 0;
        for (auto& argument : pFunction->args())
        {
            argument.setName(arguments[i++].first);
        }
    }
} // namespace emitters
} // namespace ell
