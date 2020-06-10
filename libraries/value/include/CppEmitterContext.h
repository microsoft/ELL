////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CppEmitterContext.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ComputeContext.h"
#include "EmitterContext.h"
#include "FunctionDeclaration.h"

#include <forward_list>
#include <functional>
#include <iosfwd>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace ell
{
namespace value
{
    class CppEmitterContext : public EmitterContext
    {
    public:
        CppEmitterContext(std::string moduleName, std::ostream& stream);
        CppEmitterContext(std::string modulename, std::unique_ptr<std::ostream> stream);
        CppEmitterContext(const TargetDevice& target, std::string moduleName, std::ostream& stream);
        CppEmitterContext(const TargetDevice& target, std::string modulename, std::unique_ptr<std::ostream> stream);

        ~CppEmitterContext();

    private:
        Value AllocateImpl(ValueType, MemoryLayout, size_t /* alignment */, AllocateFlags flags) override;
        Value AllocateImpl(detail::ValueTypeDescription, std::optional<MemoryLayout>, std::string, std::optional<std::string> = std::nullopt, bool = false);

        std::optional<Value> GetGlobalValue(GlobalAllocationScope scope, std::string name) override;
        Value GlobalAllocateImpl(GlobalAllocationScope scope, std::string name, ConstantData data, MemoryLayout layout, AllocateFlags flags) override;
        Value GlobalAllocateImpl(GlobalAllocationScope scope, std::string name, ValueType type, MemoryLayout layout, AllocateFlags flags) override;

        detail::ValueTypeDescription GetTypeImpl(Emittable) override;

        DefinedFunction CreateFunctionImpl(FunctionDeclaration decl, DefinedFunction fn) override;
        bool IsFunctionDefinedImpl(FunctionDeclaration decl) const override;

        Value StoreConstantDataImpl(ConstantData data) override;

        void ForImpl(MemoryLayout layout, std::function<void(std::vector<Scalar>)> fn, const std::string& name) override;
        void ForImpl(Scalar start, Scalar stop, Scalar step, std::function<void(Scalar)> fn, const std::string& name) override;

        void MoveDataImpl(Value& source, Value& destination) override;

        void CopyDataImpl(const Value& source, Value& destination) override;

        Value ReferenceImpl(Value source) override;

        Value DereferenceImpl(Value source) override;

        Value OffsetImpl(Value source, Value offset) override;

        Value UnaryOperationImpl(ValueUnaryOperation op, Value destination) override;
        Value BinaryOperationImpl(ValueBinaryOperation op, Value destination, Value source) override;

        Value LogicalOperationImpl(ValueLogicalOperation op, Value source1, Value source2) override;

        Value CastImpl(Value value, ValueType type) override;

        IfContext IfImpl(Scalar test, std::function<void()> fn) override;

        void WhileImpl(Scalar test, std::function<void()> fn) override;

        std::optional<Value> CallImpl(FunctionDeclaration func, std::vector<Value> args) override;

        void PrefetchImpl(Value data, PrefetchType type, PrefetchLocality locality) override;

        void ParallelizeImpl(int numTasks, std::vector<Value> captured, std::function<void(Scalar, std::vector<Value>)> fn) override;

        void DebugBreakImpl() override;
        void DebugDumpImpl(Value value, std::string tag, std::ostream& stream) const override;
        void DebugDumpImpl(FunctionDeclaration fn, std::string tag, std::ostream& stream) const override;
        void DebugPrintImpl(std::string message) override;

        void SetNameImpl(const Value& value, const std::string& name) override;
        std::string GetNameImpl(const Value& value) const override;

        void ImportCodeFileImpl(std::string) override;

        Scalar GetFunctionAddressImpl(const FunctionDeclaration& fn) override;

        Value IntrinsicCall(FunctionDeclaration intrinsic, std::vector<Value> args);
        std::optional<Value> EmitExternalCall(FunctionDeclaration externalFunc, std::vector<Value> args);

        void DeclareFunction(FunctionDeclaration decl);
        std::ostream& WriteFunctionSignature(std::ostream& stream, FunctionDeclaration decl);

        std::ostream& Out();
        std::ostream& Global();
        std::ostream& FnDecl();

        std::string GetScopeAdjustedName(GlobalAllocationScope scope, std::string name) const;
        std::string GetGlobalScopedName(std::string name) const;
        std::string GetCurrentFunctionScopedName(std::string name) const;

        Value SimpleNumericIntrinsic(FunctionDeclaration intrinsic, std::vector<Value> args);
        Value MaxMinIntrinsic(FunctionDeclaration intrinsic, std::vector<Value> args);
        Value PowIntrinsic(FunctionDeclaration intrinsic, std::vector<Value> args);
        Value CopySignIntrinsic(FunctionDeclaration intrinsic, std::vector<Value> args);
        Value FmaIntrinsic(FunctionDeclaration intrinsic, std::vector<Value> args);
        Value MemFnIntrinsic(FunctionDeclaration intrinsic, std::vector<Value> args);

        template <typename Fn>
        void Indented(Fn&&);

        struct PromotedConstantDataDescription
        {
            const ConstantData* data;
            Emittable realValue;
        };

        Value PromoteConstantData(Value value);
        std::optional<PromotedConstantDataDescription> HasBeenPromoted(Value value) const;
        Value Realize(Value value);
        Value EnsureEmittable(Value value);

        std::string ScalarToString(ViewAdapter scalar) const;

        struct ValueImpl
        {
            std::string name;
            detail::ValueTypeDescription typeDesc;
        };

        struct FnContext
        {
            std::forward_list<ValueImpl> dataList;
            std::string name;
        };

        class IfContextImpl;
        struct FunctionScope;

        std::unique_ptr<std::ostream> _ownedStream;
        ComputeContext _computeContext;

        std::stack<std::vector<PromotedConstantDataDescription>> _promotedConstantStack;
        std::stack<FnContext> _fnStacks;
        std::ostringstream _globalStream;
        std::ostringstream _fnDeclStream;
        std::ostringstream _expressionStream;
        std::reference_wrapper<std::ostream> _stream;
        std::reference_wrapper<std::ostream> _outputStream;
        std::unordered_map<FunctionDeclaration, DefinedFunction> _definedFunctions;
        std::map<std::string, std::pair<Emittable, MemoryLayout>> _globals;
        std::forward_list<ValueImpl> _globalsList;
        std::unordered_set<std::string> _declaredFunctions;
        std::string _moduleName;
        size_t _indent = 0;
    };

} // namespace value
} // namespace ell
