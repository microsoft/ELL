////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Variable.h (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "EmitterTypes.h"

// utilities
#include "IntegerStack.h"

// stl
#include <memory>
#include <string>
#include <vector>

namespace ell
{
namespace emitters
{
    /// <summary> Metadata about an emitted variable. </summary>
    struct EmittedVariable
    {
        /// <summary> Has this variable been declared already? </summary>
        bool isNew = false;

        /// <summary> Variable # </summary>
        size_t varIndex = 0;

        /// <summary> Set fields to default </summary>
        void Clear();
    };

    /// <summary> Allocator to allocate, free and reuse emitted (physical) variables </summary>
    class EmittedVariableAllocator
    {
    public:
        /// <summary> Alloc a variable </summary>
        EmittedVariable Allocate();

        /// <summary> Free a variable </summary>
        void Free(EmittedVariable& var);

    private:
        utilities::IntegerStack _varStack;
    };

    /// <summary> Our compilers work with scoped variables </summary>
    enum class VariableScope
    {
        literal, /// <summary> Literal variable </summary>
        local, /// <summary> Local (stack) variable </summary>
        global, /// <summary> Global variable </summary>
        heap, /// <summary> Heap allocated variable </summary>
        rValue, /// <summary> Variable returned by a helper function </summary>
        input, /// <summary> Input function argument </summary>
        output /// <summary> Output function argument </summary>

    };

    /// <summary>
    /// ELL Model output ports are bound to logical variables. Variables represent ports and port elements in machine terms: the local,
    /// global or heap scalars and vectors that they are being emitted as.
    /// Our compilers can emit code/IR in multiple formats. Hence we need a format independant scheme
    ///  </summary>
    class Variable
    {
    public:
        /// <summary> Variable options </summary>
        enum VariableFlags
        {
            none = 0,
            isMutable = 0x00000001, /// <summary> Mutable or constant </summary>
            hasInitValue = 0x00000002, /// <summary> Initialized or not </summary>
            isVectorRef = 0x00000004 /// <summary> Is this a offset into a vector or array </summary>
        };

    public:
        /// <summary> Variable data type </summary>
        VariableType Type() const { return _type; }

        /// <summary> Variable scope </summary>
        VariableScope Scope() const { return _scope; }

        /// <summary> Variable dimension - scalar or vector </summary>
        virtual size_t Dimension() const { return 1; }

        /// <summary> Is this a vector variable? </summary>
        bool IsVector() const { return !IsScalar(); }

        /// <summary> Is this a scalar variable? </summary>
        virtual bool IsScalar() const { return Dimension() == 1; }

        /// <summary> When a variable is emitted, it is bound to a name </summary>
        const std::string& EmittedName() const { return _emittedName; }

        /// <summary> Was the variable emitted? </summary>
        bool HasEmittedName() const { return _emittedName.length() > 0; }

        /// <summary> Set the emitted name for the variable </summary>
        void SetEmittedName(std::string emittedName);

        /// <summary> Is this a literal variable? </summary>
        bool IsLiteral() const { return _scope == VariableScope::literal; }

        bool IsInputArgument() const { return _scope == VariableScope::input; }
        
        /// <summary> Is this a global variable? </summary>
        bool IsGlobal() const { return _scope == VariableScope::global; }

        /// <summary> Is this an RValue variable? </summary>
        bool IsRValue() const { return _scope == VariableScope::rValue; }

        /// <summary> Is this a variable mutable? </summary>
        bool IsMutable() const { return TestFlags(VariableFlags::isMutable); }

        /// <summary> Is this a variable a constant? </summary>
        bool IsConstant() const { return !IsMutable(); }

        /// <summary> Is this variable a reference into a vector? </summary>
        bool IsVectorRef() const { return TestFlags(VariableFlags::isVectorRef); }

        /// <summary> Does the variable need to be initialized? </summary>
        bool HasInitValue() const { return TestFlags(VariableFlags::hasInitValue); }

        /// <summary> True if this a new variable. False it has it already been declared? </summary>
        bool IsNew() const { return _emittedVar.isNew; }

        /// <summary> Test if the given set of flags are set </summary>
        bool TestFlags(int flags) const;

        /// <summary> Bind the logical variable to a physical one </summary>
        void AssignVariable(EmittedVariable variable);

        /// <summary> Get the physical variable bound to this logical variable </summary>
        EmittedVariable GetAssignedVar() { return _emittedVar; }

    protected:
        Variable(const VariableType type, const VariableScope scope, int flags = VariableFlags::none);
        void SetFlags(const VariableFlags flag);
        void ClearFlags(const VariableFlags flag);

    private:
        std::string _emittedName;
        VariableType _type;
        VariableScope _scope;
        int _flags;
        EmittedVariable _emittedVar;
    };

    /// <summary> Allocator for logical variables </summary>
    class VariableAllocator
    {
    public:
        /// <summary> Add variable of a given type </summary>
        template <typename VarType, typename... Args>
        VarType* AddVariable(Args&&... args);

        /// <summary> Add a scalar, initialized to zero </summary>
        Variable* AddScalarVariable(VariableScope scope, VariableType type);

        /// <summary> Add a scalar, initialized to zero </summary>
        template <typename ElementType>
        Variable* AddScalarVariable(VariableScope scope);

        /// <summary> Add a scalar, initialized to a given value </summary>
        template <typename ElementType>
        Variable* AddScalarVariable(VariableScope scope, ElementType value);

        /// <summary> Add a vector, initialized to zero </summary>
        Variable* AddVectorVariable(VariableScope scope, VariableType type, int size);

        /// <summary> Add a vector, initialized to zero </summary>
        template <typename ElementType>
        Variable* AddVectorVariable(VariableScope scope, int size);

        /// <summary> Add a vector, with all elements initialized to a given value </summary>
        template <typename ElementType>
        Variable* AddVectorVariable(VariableScope scope, int size, ElementType value);

        /// <summary> Add a vector, initialized to a given vector </summary>
        template <typename ElementType>
        Variable* AddVectorVariable(VariableScope scope, const std::vector<ElementType>& values);

        /// <summary> Add a reference to vector element </summary>
        Variable* AddVectorElementVariable(VariableType type, Variable& src, int offset);

    private:
        std::vector<std::shared_ptr<Variable>> _variables;
    };
}
}

#include "../tcc/Variable.tcc"
