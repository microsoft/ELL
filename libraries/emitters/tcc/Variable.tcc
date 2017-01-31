////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Variable.tcc (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace emitters
{
    template <typename VarType, typename... Args>
    VarType* VariableAllocator::AddVariable(Args&&... args)
    {
        static_assert(std::is_base_of<Variable, VarType>::value, "AddVariable requires you inherit from Variable");

        auto var = std::make_shared<VarType>(args...);
        VarType* pVar = var.get();
        _variables.push_back(var);
        return pVar;
    }
}
}
