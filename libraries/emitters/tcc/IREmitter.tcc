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
    template <typename ValueType>
    llvm::Constant* IREmitter::Pointer(ValueType* ptr)
    {
        auto ptrValue = Literal(reinterpret_cast<int64_t>(ptr));
        auto ptrType = PointerType(emitters::GetVariableType<ValueType>());
        return llvm::ConstantExpr::getIntToPtr(ptrValue, ptrType);
    }

    template <typename ListType>
    void IREmitter::BindArgumentNames(llvm::Function* pFunction, const ListType& arguments)
    {
        size_t i = 0;
        for (auto& argument : pFunction->args())
        {
            argument.setName(arguments[i++].first);
        }
    }
}
}
