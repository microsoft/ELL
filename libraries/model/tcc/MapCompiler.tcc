////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MapCompiler.tcc (model)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace model
{
    template <typename ValueType>
    emitters::Variable* MapCompiler::AllocatePortVariable(const OutputPortBase& port, ValueType initialValue)
    {
        auto pModuleEmitter = GetModuleEmitter();
        assert(port.Size() != 0);

        emitters::VariableType varType = PortTypeToVariableType(port.GetType());
        emitters::Variable* pVar = nullptr;
        const bool isScalar = port.Size() == 1;
        if (isScalar) // TODO: only do this if scope != output (or, only if scope == input or local?)
        {
            if (initialValue == 0)
            {
                pVar = pModuleEmitter->Variables().AddScalarVariable(emitters::VariableScope::local, varType);
            }
            else
            {
                pVar = pModuleEmitter->Variables().AddScalarVariable(emitters::VariableScope::local, initialValue);
            }
        }
        else
        {
            if (initialValue == 0)
            {
                pVar = pModuleEmitter->Variables().AddVectorVariable(emitters::VariableScope::global, varType, port.Size());
            }
            else
            {
                pVar = pModuleEmitter->Variables().AddVectorVariable(emitters::VariableScope::global, port.Size(), initialValue);
            }
        }

        pModuleEmitter->AllocateVariable(*pVar);
        SetVariableForPort(port, pVar);
        return pVar;
    }

    template <typename ValueType>
    emitters::Variable* MapCompiler::GetOrAllocatePortVariable(const OutputPortBase& port, ValueType initialValue)
    {
        emitters::Variable* pVar = GetVariableForPort(port);
        if (pVar == nullptr)
        {
            pVar = AllocatePortVariable(port);
        }
        assert(pVar != nullptr);
        return pVar;
    }
}
}
