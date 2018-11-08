////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ComputeContext.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterContext.h"
#include "ValueScalar.h"

// stl
#include <forward_list>
#include <map>
#include <stack>
#include <string>

namespace ell
{
namespace value
{

    /// <summary> A specialization of EmitterContext designed to execute statements directly </summary>
    class ComputeContext : public EmitterContext
    {
    public:
        /// <summary> Constructor </summary>
        /// <param name="moduleName"> The name of the module that this context represents </param>
        ComputeContext(std::string moduleName);

    private:
        Value AllocateImpl(ValueType type, MemoryLayout layout) override;

        std::pair<ValueType, int> GetTypeImpl(Emittable emittable) override;

        std::function<void()> CreateFunctionImpl(std::string fnName, std::function<void()> fn) override;
        std::function<Value()> CreateFunctionImpl(std::string fnName, Value returnValue,
                                                  std::function<Value()> fn) override;
        std::function<void(std::vector<Value>)> CreateFunctionImpl(std::string fnName, std::vector<Value> argValues,
                                                                   std::function<void(std::vector<Value>)> fn) override;
        std::function<Value(std::vector<Value>)> CreateFunctionImpl(
            std::string fnName, Value returnValue, std::vector<Value> argValues,
            std::function<Value(std::vector<Value>)> fn) override;

        Value StoreConstantDataImpl(ConstantData data) override;

        void ForImpl(MemoryLayout layout, std::function<void(std::vector<Scalar>)> fn) override;

        void MoveDataImpl(Value& source, Value& destination) override;

        void CopyDataImpl(const Value& source, Value& destination) override;

        Value OffsetImpl(Value begin, Value index) override;

        Value UnaryOperationImpl(ValueUnaryOperation op, Value destination) override;
        Value BinaryOperationImpl(ValueBinaryOperation op, Value destination, Value source) override;

        bool ValidateValue(Value value);
        bool TypeCompatible(Value value1, Value value2);

        ConstantData ExtractConstantData(Value value);
        struct FunctionScope;

        using ConstantDataList = std::forward_list<ConstantData>;

        std::stack<ConstantDataList> _stack;
        std::map<std::string, ConstantData> _globals;
        std::string _moduleName;
    };

} // namespace value
} // namespace ell