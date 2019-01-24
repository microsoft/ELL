////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BroadcastFunctionNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ConstantNode.h"

#include <emitters/include/IRAsyncTask.h>
#include <emitters/include/IREmitter.h>
#include <emitters/include/IRVectorUtilities.h>
#include <emitters/include/LLVMUtilities.h>

#include <model/include/CompilableNode.h>
#include <model/include/CompilableNodeUtilities.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>
#include <model/include/PortMemoryLayout.h>

#include <utilities/include/Exception.h>
#include <utilities/include/TypeName.h>

#include <functional>
#include <numeric>
#include <string>
#include <type_traits>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary>
    ///
    /// Broadcast function nodes perform elementwise operations on a multidimensional array (the "primary input"), and one or more vectors (the "secondary inputs").
    /// The vector input is considered to live along one dimension of the primary input, and is (virtually) replicated along the other dimensions to match the primary
    /// input size. For instance, if the primary input is a 2x3x4 the secondary input S could be a vector of size 3 along dimension 1. Then a virtual 2x3x4 SA array is
    /// generated, where entry SA[.,i,.] = S[i].
    ///
    /// BroadcastBinaryFunctionNodes perform elementwise operations of the form out = f(x, a), where x is an element from the primary input, and a is an element
    /// from the secondary input.
    ///
    /// BroadcastTernaryFunctionNodes perform elementwise operations of the form out = f(x, a, b), where x is an element from the primary input, and a and b are
    /// elements from the secondary inputs. Both secondardy inputs must lie along the same dimension.
    ///
    /// BroadcastUnaryFunctionNodes don't really broadcast anything, but perform unary operations of the form out = f(x), where x is an element from the primary input.
    /// There are no secondary inputs.
    ///
    /// </summary>
    template <typename ValueType, typename FunctionType>
    class BroadcastFunctionNode;

    //
    // Base class for unary function types
    //
    template <typename ValueType>
    class BroadcastUnaryFunction
    {
    public:
        BroadcastUnaryFunction() = default;
        BroadcastUnaryFunction(const BroadcastUnaryFunction&) = default;
        virtual ~BroadcastUnaryFunction() = default;

        /// <summary> Computes a value (on the host machine) </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <returns> The value the function f(x) </returns>
        virtual ValueType Compute(ValueType x) const = 0;

        /// <summary> Computes a value (on the host machine) </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="secondaryArgs"> The secondary values. Must have a size of 0. </param>
        /// <returns> The value the function f(x) </returns>
        ValueType Compute(ValueType x, const std::vector<ValueType>& secondaryArgs) const;

        /// <summary> Emits IR to compute a value </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <returns> The value the function f(x) </returns>
        virtual emitters::LLVMValue Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue x) const = 0;

        /// <summary> Emits IR to compute a value </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="secondaryArgs"> The secondary values. Must be empty. </param>
        /// <returns> The value the function f(x) </returns>
        emitters::LLVMValue Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue x, const std::vector<emitters::LLVMValue>& secondaryArgs) const;

        /// <summary> Indicates if the function can operate on vector types </summary>
        bool CanUseVectorTypes() const { return false; }
    };

    //
    // Base class for binary function types
    //
    template <typename ValueType>
    class BroadcastBinaryFunction
    {
    public:
        BroadcastBinaryFunction() = default;
        BroadcastBinaryFunction(const BroadcastBinaryFunction&) = default;
        virtual ~BroadcastBinaryFunction() = default;

        /// <summary> Computes a value (on the host machine) </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="a"> The secondary value </param>
        /// <returns> The value the function f(x,a) </returns>
        virtual ValueType Compute(ValueType x, ValueType a) const = 0;

        /// <summary> Computes a value (on the host machine) </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="secondaryArgs"> The secondary values. Must have size 1. </param>
        /// <returns> The value the function f(x,a) </returns>
        ValueType Compute(ValueType x, const std::vector<ValueType>& secondaryArgs) const;

        /// <summary> Emits IR to compute a value </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="a"> The secondary value </param>
        /// <returns> The value the function f(x,a) </returns>
        virtual emitters::LLVMValue Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue x, emitters::LLVMValue a) const = 0;

        /// <summary> Emits IR to compute a value </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="secondaryArgs"> The secondary values. Must have size 1. </param>
        /// <returns> The value the function f(x,a) </returns>
        emitters::LLVMValue Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue x, const std::vector<emitters::LLVMValue>& secondaryArgs) const;

        /// <summary> Indicates if the function can operate on vector types </summary>
        bool CanUseVectorTypes() const { return false; }
    };

    //
    // Base class for ternary function types
    //
    template <typename ValueType>
    class BroadcastTernaryFunction
    {
    public:
        BroadcastTernaryFunction() = default;
        BroadcastTernaryFunction(const BroadcastTernaryFunction&) = default;
        virtual ~BroadcastTernaryFunction() = default;

        /// <summary> Computes a value (on the host machine) </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="a"> The first secondary value </param>
        /// <param name="b"> The second secondary value </param>
        /// <returns> The value the function f(x,a,b) </returns>
        virtual ValueType Compute(ValueType x, ValueType a, ValueType b) const = 0;

        /// <summary> Computes a value (on the host machine) </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="secondaryArgs"> The secondary values. Must have size 2. </param>
        /// <returns> The value the function f(x,a) </returns>
        ValueType Compute(ValueType x, const std::vector<ValueType>& secondaryArgs) const;

        /// <summary> Emits IR to compute a value </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="a"> The first secondary value </param>
        /// <param name="b"> The second secondary value </param>
        /// <returns> The value the function f(x,a,b) </returns>
        virtual emitters::LLVMValue Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue x, emitters::LLVMValue a, emitters::LLVMValue b) const = 0;

        /// <summary> Emits IR to compute a value </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="secondaryArgs"> The secondary values. Must have size 2. </param>
        /// <returns> The value the function f(x,a) </returns>
        emitters::LLVMValue Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue x, const std::vector<emitters::LLVMValue>& secondaryArgs) const;

        /// <summary> Indicates if the function can operate on vector types </summary>
        bool CanUseVectorTypes() const { return false; }
    };

    //
    // Special type of ternary function: the linear function  y = x*a + b
    //
    template <typename ValueType>
    class BroadcastLinearFunction : public BroadcastTernaryFunction<ValueType>
    {
    public:
        BroadcastLinearFunction() = default;
        BroadcastLinearFunction(const BroadcastLinearFunction&) = default;

        /// <summary> Computes a linear function (on the host machine) </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="a"> The first secondary value </param>
        /// <param name="b"> The second secondary value </param>
        /// <returns> The value the function f(x,a,b) = ax + b </returns>
        ValueType Compute(ValueType x, ValueType a, ValueType b) const override;
        using BroadcastTernaryFunction<ValueType>::Compute;

        /// <summary> Emits IR to compute a value </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="a"> The first secondary value </param>
        /// <param name="b"> The second secondary value </param>
        /// <returns> The value the function f(x,a,b) = ax + b </returns>
        emitters::LLVMValue Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue x, emitters::LLVMValue a, emitters::LLVMValue b) const override;
        using BroadcastTernaryFunction<ValueType>::Compile;

        /// <summary> Indicates if the function can operate on vector types </summary>
        bool CanUseVectorTypes() const { return true; }
    };

    //
    // Base class for broadcast nodes
    //
    template <typename ValueType, typename FunctionType>
    class BroadcastFunctionNode : public model::CompilableNode
    {
    public:
        /// <summary> Returns the size of the primary input. </summary>
        virtual int GetPrimaryInputSize() const = 0;

        /// <summary> Returns the size of the secondary inputs. </summary>
        virtual int GetSecondaryInputSize() const = 0;

        /// <summary> Returns the number of secondary input ports. </summary>
        virtual int NumSecondaryInputs() const = 0;

        const model::PortMemoryLayout& GetInputMemoryLayout() const { return _inputLayout; }
        model::PortMemoryLayout GetOutputMemoryLayout() const;

        /// <summary> Returns true if the node can accept input with this memory layout order, else false </summary>
        ///
        /// <param name="order"> The memory layout order for all the input ports </summary>
        /// <returns> If the node can accept the input memory layout order, true, else false </returns>
        bool CanAcceptInputLayout(const utilities::DimensionOrder& order) const override
        {
            return GetInputMemoryLayout().GetLogicalDimensionOrder() == order;
        }

        size_t GetBroadcastDimension() const { return _broadcastDimension; }
        size_t NumPrimaryInputDimensions() const { return GetInputMemoryLayout().NumDimensions(); }

    protected:
        BroadcastFunctionNode(const std::vector<model::InputPortBase*>& inputs, const std::vector<model::OutputPortBase*>& outputs);

        BroadcastFunctionNode(const std::vector<model::InputPortBase*>& inputs,
                              const model::PortMemoryLayout& inputLayout,
                              size_t broadcastDimension,
                              const std::vector<model::OutputPortBase*>& outputs,
                              const model::PortMemoryLayout& outputLayout,
                              FunctionType function,
                              ValueType padding = 0);

        bool HasState() const override { return true; } // stored state: function, broadcast dimension, and padding value

        virtual const model::InputPort<ValueType>& GetPrimaryInput() const = 0;
        virtual const model::InputPort<ValueType>* GetSecondaryInput(int index) const = 0;
        virtual const model::OutputPort<ValueType>& GetOutput() const = 0;
        bool IsSecondaryInputPresent(int index) const;
        FunctionType GetFunction() const { return _function; }

        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;

        // Helpers for generating nested loops to visit all input/output values
        void ComputeDimensionLoop(size_t dimension, std::vector<ValueType>& output, size_t prevInputDimensionOffset, size_t prevOutputDimensionOffset, std::vector<ValueType>& secondaryValues) const;
        void EmitComputeDimensionLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, size_t dimension, emitters::IRLocalScalar begin, emitters::IRLocalScalar end, emitters::LLVMValue primaryInput, const std::vector<emitters::LLVMValue>& secondaryInputs, emitters::LLVMValue output, emitters::IRLocalScalar prevInputDimensionOffset, emitters::IRLocalScalar prevOutputDimensionOffset, std::vector<emitters::LLVMValue>& secondaryValues) const;
        emitters::IRFunctionEmitter GetTaskFunction(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, const emitters::LLVMTypeList& portTypes) const;

        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        ValueType GetOutputPadding() const { return _paddingValue; }

    private:
        model::PortMemoryLayout _inputLayout;
        size_t _broadcastDimension = 0;

        FunctionType _function;
        ValueType _paddingValue;
    };

    //
    // BroadcastUnaryFunctionNode
    //
    template <typename ValueType, typename FunctionType>
    class BroadcastUnaryFunctionNode : public BroadcastFunctionNode<ValueType, FunctionType>
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* primaryInputPortName = "primaryInput";
        const model::InputPort<ValueType>& primaryInput = _primaryInput;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary></summary>
        BroadcastUnaryFunctionNode();

        /// <summary></summary>
        BroadcastUnaryFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout, const model::PortMemoryLayout& outputLayout, ValueType padding = 0);

        /// <summary></summary>
        BroadcastUnaryFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout, const model::PortMemoryLayout& outputLayout, FunctionType function, ValueType padding = 0);

        /// <summary></summary>
        int GetPrimaryInputSize() const override { return _primaryInput.Size(); }

        /// <summary></summary>
        int GetSecondaryInputSize() const override { return 0; }

        /// <summary></summary>
        int NumSecondaryInputs() const override { return 0; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, FunctionType>("BroadcastUnaryFunctionNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        using BroadcastFunctionNode<ValueType, FunctionType>::GetInputMemoryLayout;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetOutputMemoryLayout;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetBroadcastDimension;
        using BroadcastFunctionNode<ValueType, FunctionType>::NumPrimaryInputDimensions;

    protected:
        using BroadcastFunctionNode<ValueType, FunctionType>::GetFunction;

        utilities::ArchiveVersion GetArchiveVersion() const override;
        bool CanReadArchiveVersion(const utilities::ArchiveVersion& version) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        const model::InputPort<ValueType>& GetPrimaryInput() const override { return primaryInput; }
        const model::InputPort<ValueType>* GetSecondaryInput(int index) const override;
        const model::OutputPort<ValueType>& GetOutput() const override { return output; }

    private:
        using BroadcastFunctionNode<ValueType, FunctionType>::ComputeDimensionLoop;
        using BroadcastFunctionNode<ValueType, FunctionType>::EmitComputeDimensionLoop;

        void Copy(model::ModelTransformer& transformer) const override;

        // Inputs
        model::InputPort<ValueType> _primaryInput;

        // Output
        model::OutputPort<ValueType> _output;
    };

    //
    // BroadcastBinaryFunctionNode
    //
    template <typename ValueType, typename FunctionType>
    class BroadcastBinaryFunctionNode : public BroadcastFunctionNode<ValueType, FunctionType>
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* primaryInputPortName = "primaryInput";
        static constexpr const char* secondaryInputPortName = "secondaryInput";
        const model::InputPort<ValueType>& primaryInput = _primaryInput;
        const model::InputPort<ValueType>& secondaryInput = _secondaryInput;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary></summary>
        BroadcastBinaryFunctionNode();

        /// <summary></summary>
        BroadcastBinaryFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout, const model::OutputPort<ValueType>& secondaryInput, size_t secondaryInputDimension, const model::PortMemoryLayout& outputLayout, ValueType padding = 0);

        /// <summary></summary>
        BroadcastBinaryFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout, const model::OutputPort<ValueType>& secondaryInput, size_t secondaryInputDimension, const model::PortMemoryLayout& outputLayout, FunctionType function, ValueType padding = 0);

        /// <summary></summary>
        int GetPrimaryInputSize() const override { return _primaryInput.Size(); }

        /// <summary></summary>
        int GetSecondaryInputSize() const override { return _secondaryInput.Size(); }

        /// <summary></summary>
        int NumSecondaryInputs() const override { return 1; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BroadcastBinaryFunctionNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        using BroadcastFunctionNode<ValueType, FunctionType>::GetInputMemoryLayout;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetOutputMemoryLayout;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetBroadcastDimension;
        using BroadcastFunctionNode<ValueType, FunctionType>::NumPrimaryInputDimensions;

    protected:
        using BroadcastFunctionNode<ValueType, FunctionType>::GetFunction;
        using BroadcastFunctionNode<ValueType, FunctionType>::NumElements;

        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        const model::InputPort<ValueType>& GetPrimaryInput() const override { return primaryInput; }
        const model::InputPort<ValueType>* GetSecondaryInput(int index) const override;
        const model::OutputPort<ValueType>& GetOutput() const override { return output; }

    private:
        using BroadcastFunctionNode<ValueType, FunctionType>::ComputeDimensionLoop;
        using BroadcastFunctionNode<ValueType, FunctionType>::EmitComputeDimensionLoop;

        void Copy(model::ModelTransformer& transformer) const override;

        // Inputs
        model::InputPort<ValueType> _primaryInput;
        model::InputPort<ValueType> _secondaryInput;

        // Output
        model::OutputPort<ValueType> _output;
    };

    //
    // BroadcastTernaryFunctionNode
    //
    template <typename ValueType, typename FunctionType>
    class BroadcastTernaryFunctionNode : public BroadcastFunctionNode<ValueType, FunctionType>
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* primaryInputPortName = "primaryInput";
        static constexpr const char* secondaryInput1PortName = "secondaryInput1";
        static constexpr const char* secondaryInput2PortName = "secondaryInput2";
        const model::InputPort<ValueType>& primaryInput = _primaryInput;
        const model::InputPort<ValueType>& secondaryInput1 = _secondaryInput1;
        const model::InputPort<ValueType>& secondaryInput2 = _secondaryInput2;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary></summary>
        BroadcastTernaryFunctionNode();

        /// <summary></summary>
        BroadcastTernaryFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout, const model::OutputPort<ValueType>& secondaryInput1, const model::OutputPort<ValueType>& secondaryInput2, size_t secondaryInputDimension, const model::PortMemoryLayout& outputLayout, ValueType padding = 0);

        /// <summary></summary>
        BroadcastTernaryFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout, const model::OutputPort<ValueType>& secondaryInput1, const model::OutputPort<ValueType>& secondaryInput2, size_t secondaryInputDimension, const model::PortMemoryLayout& outputLayout, FunctionType function, ValueType padding = 0);

        /// <summary></summary>
        int GetPrimaryInputSize() const override { return _primaryInput.Size(); }

        /// <summary></summary>
        int GetSecondaryInputSize() const override { return std::max(_secondaryInput1.Size(), _secondaryInput2.Size()); }

        /// <summary></summary>
        int NumSecondaryInputs() const override { return 2; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BroadcastTernaryFunctionNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        using BroadcastFunctionNode<ValueType, FunctionType>::GetInputMemoryLayout;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetOutputMemoryLayout;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetBroadcastDimension;
        using BroadcastFunctionNode<ValueType, FunctionType>::NumPrimaryInputDimensions;

    protected:
        using BroadcastFunctionNode<ValueType, FunctionType>::GetFunction;

        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        const model::InputPort<ValueType>& GetPrimaryInput() const override { return primaryInput; }
        const model::InputPort<ValueType>* GetSecondaryInput(int index) const override;
        const model::OutputPort<ValueType>& GetOutput() const override { return output; }

    private:
        using BroadcastFunctionNode<ValueType, FunctionType>::ComputeDimensionLoop;
        using BroadcastFunctionNode<ValueType, FunctionType>::EmitComputeDimensionLoop;

        void Copy(model::ModelTransformer& transformer) const override;

        // Inputs
        model::InputPort<ValueType> _primaryInput;
        model::InputPort<ValueType> _secondaryInput1;
        model::InputPort<ValueType> _secondaryInput2;

        // Output
        model::OutputPort<ValueType> _output;
    };

    //
    // Special case of BroadcastTernaryFunctionNode, using a linear function
    //
    template <typename ValueType>
    class BroadcastLinearFunctionNode : public BroadcastTernaryFunctionNode<ValueType, BroadcastLinearFunction<ValueType>>
    {
    public:
        using BroadcastTernaryFunctionNode<ValueType, BroadcastLinearFunction<ValueType>>::primaryInput;
        using BroadcastTernaryFunctionNode<ValueType, BroadcastLinearFunction<ValueType>>::secondaryInput1;
        using BroadcastTernaryFunctionNode<ValueType, BroadcastLinearFunction<ValueType>>::secondaryInput2;
        using BroadcastTernaryFunctionNode<ValueType, BroadcastLinearFunction<ValueType>>::output;

        /// <summary></summary>
        BroadcastLinearFunctionNode();

        /// <summary></summary>
        BroadcastLinearFunctionNode(const model::OutputPort<ValueType>& primaryInput,
                                    const model::PortMemoryLayout& inputLayout,
                                    const model::OutputPort<ValueType>& scaleInput,
                                    const model::OutputPort<ValueType>& biasInput,
                                    size_t secondaryInputDimension,
                                    const model::PortMemoryLayout& outputLayout,
                                    ValueType padding = 0);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BroadcastLinearFunctionNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        bool HasState() const override { return false; }
        bool HasScale() const { return secondaryInput1.Size() != 0; }
        bool HasBias() const { return secondaryInput2.Size() != 0; }

    private:
        void Copy(model::ModelTransformer& transformer) const override;
    };
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    //
    // BroadcastUnaryFunction
    //
    template <typename ValueType>
    ValueType BroadcastUnaryFunction<ValueType>::Compute(ValueType x, const std::vector<ValueType>& secondaryArgs) const
    {
        assert(secondaryArgs.size() == 0);
        return Compute(x);
    }

    template <typename ValueType>
    emitters::LLVMValue BroadcastUnaryFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue x, const std::vector<emitters::LLVMValue>& secondaryArgs) const
    {
        assert(secondaryArgs.size() == 0);
        return this->Compile(function, x);
    }

    //
    // BroadcastBinaryFunction
    //
    template <typename ValueType>
    ValueType BroadcastBinaryFunction<ValueType>::Compute(ValueType x, const std::vector<ValueType>& secondaryArgs) const
    {
        assert(secondaryArgs.size() == 1);
        return Compute(x, secondaryArgs[0]);
    }

    template <typename ValueType>
    emitters::LLVMValue BroadcastBinaryFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue x, const std::vector<emitters::LLVMValue>& secondaryArgs) const
    {
        assert(secondaryArgs.size() == 1);
        return this->Compile(function, x, secondaryArgs[0]);
    }

    //
    // BroadcastTernaryFunction
    //
    template <typename ValueType>
    ValueType BroadcastTernaryFunction<ValueType>::Compute(ValueType x, const std::vector<ValueType>& secondaryArgs) const
    {
        assert(secondaryArgs.size() == 2);
        return Compute(x, secondaryArgs[0], secondaryArgs[1]);
    }

    template <typename ValueType>
    emitters::LLVMValue BroadcastTernaryFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue x, const std::vector<emitters::LLVMValue>& secondaryArgs) const
    {
        assert(secondaryArgs.size() == 2);
        return this->Compile(function, x, secondaryArgs[0], secondaryArgs[1]);
    }

    //
    // BroadcastLinearFunction
    //
    template <typename ValueType>
    ValueType BroadcastLinearFunction<ValueType>::Compute(ValueType x, ValueType scale, ValueType bias) const
    {
        return scale * x + bias;
    }

    template <typename ValueType>
    emitters::LLVMValue BroadcastLinearFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue x, emitters::LLVMValue scale, emitters::LLVMValue bias) const
    {
        if (scale == nullptr) // bias only
        {
            return function.Operator(emitters::GetAddForValueType<ValueType>(), x, bias);
        }
        else if (bias == nullptr) // scale only
        {
            return function.Operator(emitters::GetMultiplyForValueType<ValueType>(), scale, x);
        }
        else
        {
            return function.Operator(emitters::GetAddForValueType<ValueType>(), function.Operator(emitters::GetMultiplyForValueType<ValueType>(), scale, x), bias);
        }
    }

    //
    // BroadcastFunctionNode
    //

    template <typename ValueType, typename FunctionType>
    BroadcastFunctionNode<ValueType, FunctionType>::BroadcastFunctionNode(const std::vector<model::InputPortBase*>& inputs, const std::vector<model::OutputPortBase*>& outputs) :
        CompilableNode(inputs, outputs),
        _paddingValue(0)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastFunctionNode<ValueType, FunctionType>::BroadcastFunctionNode(const std::vector<model::InputPortBase*>& inputs,
                                                                          const model::PortMemoryLayout& inputLayout,
                                                                          size_t broadcastDimension,
                                                                          const std::vector<model::OutputPortBase*>& outputs,
                                                                          const model::PortMemoryLayout& outputLayout,
                                                                          FunctionType function,
                                                                          ValueType paddingValue) :
        CompilableNode(inputs, outputs),
        _inputLayout(inputLayout),
        _broadcastDimension(broadcastDimension),
        _function(function),
        _paddingValue(paddingValue)
    {
    }

    template <typename ValueType, typename FunctionType>
    model::PortMemoryLayout BroadcastFunctionNode<ValueType, FunctionType>::GetOutputMemoryLayout() const
    {
        return GetOutputPort(0)->GetMemoryLayout();
    }

    //
    // Arbitrary-depth nested loops are generated recursively. The EmitComputeDimensionLoop
    // function emits `numDimensions` nested loops of the form:
    //
    // for(iz = 0; iz < sz; ++iz)
    // {
    //     zOffset = (iz+offset[2]) * stride[2];
    //     for(iy = 0; iy < sy; ++iy)
    //     {
    //         yOffset = zOffset + (iy+offset[1]) * stride[1];
    //         for(ix = 0; ix < sx; ++ix)
    //         {
    //             offset = yOffset + (ix+offset[0]) * stride[0];
    //             x = arr[offset];
    //             val = f(x);
    //             output[offset] = val;
    //         }
    //     }
    // }
    //

    // Note: secondaryValues is passed by non-const reference to avoid copies. It doesn't function as an output parameter.
    template <typename ValueType, typename FunctionType>
    void BroadcastFunctionNode<ValueType, FunctionType>::ComputeDimensionLoop(size_t dimension, std::vector<ValueType>& output, size_t prevInputDimensionOffset, size_t prevOutputDimensionOffset, std::vector<ValueType>& secondaryValues) const
    {
        // Note: It should be easy to unroll the last K levels by putting a real loop here when dimension < k
        //       Or, instead of unrolling, vectorizing:  if broadcastDimension = 1, let secondaryValue be a vector and load it one loop previous
        //       If broadcastDimension = outermost dimension (0), we may want to parallelize over that dimension
        const auto numDimensions = NumPrimaryInputDimensions();
        auto&& inputLayout = GetInputMemoryLayout();
        auto&& inputStride = inputLayout.GetExtent();
        auto&& inputOffset = inputLayout.GetOffset();
        auto&& inputSize = inputLayout.GetActiveSize();
        auto&& outputLayout = GetOutputMemoryLayout();
        auto&& outputStride = outputLayout.GetExtent();
        auto&& outputOffset = outputLayout.GetOffset();
        auto&& primaryInput = GetPrimaryInput();
        const auto broadcastDimension = GetBroadcastDimension();
        const auto numSecondaryInputs = NumSecondaryInputs();

        for (int loopIndex = 0; loopIndex < inputSize[dimension]; ++loopIndex)
        {
            // offset within start of this dimension = (loopIndex + offset[dimension])
            auto thisInputDimensionInternalOffset = loopIndex + inputOffset[dimension];
            auto thisOutputDimensionInternalOffset = loopIndex + outputOffset[dimension];

            size_t thisInputDimensionOffset = thisInputDimensionInternalOffset;
            size_t thisOutputDimensionOffset = thisOutputDimensionInternalOffset;
            if (dimension != 0)
            {
                thisInputDimensionOffset += prevInputDimensionOffset * inputStride[dimension];
                thisOutputDimensionOffset += prevOutputDimensionOffset * outputStride[dimension];
            }

            if (dimension == broadcastDimension)
            {
                for (int index = 0; index < numSecondaryInputs; ++index)
                {
                    auto&& secondaryInput = GetSecondaryInput(index);
                    if (IsSecondaryInputPresent(index))
                    {
                        secondaryValues[index] = (*secondaryInput)[loopIndex];
                    }
                    else
                    {
                        // Dubious hack to deal with linear function nodes missing a coefficient
                        if (std::is_same<FunctionType, BroadcastLinearFunction<ValueType>>::value && index == 0) // "scale" value, which should be 1 if not specified
                        {
                            secondaryValues[index] = static_cast<ValueType>(1.0);
                        }
                        else
                        {
                            secondaryValues[index] = 0;
                        }
                    }
                }
            }

            if (dimension < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                ComputeDimensionLoop(dimension + 1, output, thisInputDimensionOffset, thisOutputDimensionOffset, secondaryValues);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                auto primaryValue = primaryInput[thisInputDimensionOffset];
                auto outputValue = GetFunction().Compute(primaryValue, secondaryValues);
                output[thisOutputDimensionOffset] = outputValue;
            }
        }
    }

    // wrapper around EmitComputeDimensionLoop for use by parallel tasks
    template <typename ValueType, typename FunctionType>
    emitters::IRFunctionEmitter BroadcastFunctionNode<ValueType, FunctionType>::GetTaskFunction(model::IRMapCompiler& compiler,
                                                                                                emitters::IRFunctionEmitter& function,
                                                                                                const emitters::LLVMTypeList& portTypes) const
    {
        auto& module = function.GetModule();
        auto& emitter = module.GetIREmitter();
        auto& context = module.GetLLVMContext();
        auto int32Type = emitter.Type(emitters::VariableType::Int32);
        auto voidType = llvm::Type::getVoidTy(context);

        // ASSUME dimension == 0 --- we're only parallelizing on the outermost loop
        int dimension = 0;

        emitters::LLVMTypeList argTypes = portTypes;
        // int numValuePorts = 2 + NumSecondaryInputs(); // primary input, secondary inputs, output
        // argTypes.insert(argTypes.end(), numValuePorts, valuePtrType);
        argTypes.insert(argTypes.end(), 2, int32Type); // begin, end

        auto taskFunction = function.GetModule().BeginFunction(utilities::to_string(GetId()) + "_task", voidType, argTypes);
        std::vector<size_t> indices(argTypes.size() - 2);
        std::iota(indices.begin(), indices.end(), 0);
        taskFunction.SetAttributeForArguments(indices, emitters::IRFunctionEmitter::Attributes::NoAlias);

        {
            // get stuff from arguments
            auto arguments = taskFunction.Arguments().begin();
            auto primaryInput = &(*arguments++);
            std::vector<emitters::LLVMValue> secondaryInputs;
            std::vector<emitters::LLVMValue> secondaryValues;
            for (int index = 0; index < NumSecondaryInputs(); ++index)
            {
                auto secondaryInput = &(*arguments++);
                // if we really have an input, push it, else push a nullptr (note: we know this at compile-time)
                if (IsSecondaryInputPresent(index))
                {
                    secondaryInputs.push_back(secondaryInput);
                }
                else
                {
                    secondaryInputs.push_back(nullptr);
                }
                secondaryValues.push_back(nullptr);
            }
            auto output = &(*arguments++);
            auto begin = function.LocalScalar(&(*arguments++));
            auto end = function.LocalScalar(&(*arguments++));
            auto prevInputDimensionOffset = function.LocalScalar();
            auto prevOutputDimensionOffset = function.LocalScalar();

            EmitComputeDimensionLoop(compiler, taskFunction, dimension, begin, end, primaryInput, secondaryInputs, output, prevInputDimensionOffset, prevOutputDimensionOffset, secondaryValues);
            taskFunction.Return();
        }
        function.GetModule().EndFunction();

        return taskFunction;
    }

    // Note: secondaryValues is passed by non-const reference to avoid copies. It doesn't function as an output parameter.
    template <typename ValueType, typename FunctionType>
    void BroadcastFunctionNode<ValueType, FunctionType>::EmitComputeDimensionLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, size_t dimension, emitters::IRLocalScalar begin, emitters::IRLocalScalar end, emitters::LLVMValue primaryInput, const std::vector<emitters::LLVMValue>& secondaryInputs, emitters::LLVMValue output, emitters::IRLocalScalar prevInputDimensionOffset, emitters::IRLocalScalar prevOutputDimensionOffset, std::vector<emitters::LLVMValue>& secondaryValues) const
    {
        // Note: It should be easy to unroll the last K levels by putting a real loop here when dimension < k
        //       Or, instead of unrolling, vectorizing --- if broadcastDimension = 1, let secondaryValue be a vector and load it one loop previous
        //       If broadcastDimension = outermost dimension (0), we may want to parallelize over that dimension
        const auto numDimensions = NumPrimaryInputDimensions();
        auto&& inputLayout = GetInputMemoryLayout();
        auto&& inputStride = inputLayout.GetExtent();
        auto&& inputOffset = inputLayout.GetOffset();
        auto&& inputSize = inputLayout.GetActiveSize();
        auto&& outputLayout = GetOutputMemoryLayout();
        auto&& outputStride = outputLayout.GetExtent();
        auto&& outputOffset = outputLayout.GetOffset();
        const auto broadcastDimension = GetBroadcastDimension();
        const auto numSecondaryInputs = NumSecondaryInputs();

        function.For(begin, end, [dimension, numDimensions, inputSize, inputOffset, inputStride, outputOffset, outputStride, broadcastDimension, numSecondaryInputs, prevInputDimensionOffset, prevOutputDimensionOffset, primaryInput, secondaryInputs, output, &secondaryValues, &compiler, this](emitters::IRFunctionEmitter& function, auto loopIndex) {
            // Calculate the offset within this dimension = (loopIndex + offset[dimension])
            auto thisInputDimensionInternalOffset = loopIndex + inputOffset[dimension];
            auto thisOutputDimensionInternalOffset = loopIndex + outputOffset[dimension];

            // Calculate the total offset from beginning of memory:
            //   * if in the outermost loop, the offset into this dimension
            //   * otherwise, the offset into this dimension plus the previous offset scaled by the previous dimension's stride
            auto thisInputDimensionOffset = function.LocalScalar();
            auto thisOutputDimensionOffset = function.LocalScalar();
            if (dimension == 0)
            {
                assert(!prevInputDimensionOffset.IsValid());
                assert(!prevOutputDimensionOffset.IsValid());
                thisInputDimensionOffset = thisInputDimensionInternalOffset;
                thisOutputDimensionOffset = thisOutputDimensionInternalOffset;
            }
            else
            {
                thisInputDimensionOffset = thisInputDimensionInternalOffset + (prevInputDimensionOffset * inputStride[dimension]);
                thisOutputDimensionOffset = thisOutputDimensionInternalOffset + (prevOutputDimensionOffset * outputStride[dimension]);
            }

            if (dimension == broadcastDimension)
            {
                for (int index = 0; index < numSecondaryInputs; ++index)
                {
                    auto&& secondaryInput = secondaryInputs[index];
                    secondaryValues[index] = this->IsSecondaryInputPresent(index) ? function.ValueAt(secondaryInput, loopIndex) : nullptr;
                }
            }

            if (dimension < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                auto nextBegin = function.LocalScalar<int>(0);
                auto nextEnd = function.LocalScalar<int>(inputSize[dimension + 1]);
                this->EmitComputeDimensionLoop(compiler, function, dimension + 1, nextBegin, nextEnd, primaryInput, secondaryInputs, output, thisInputDimensionOffset, thisOutputDimensionOffset, secondaryValues);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                auto primaryValue = function.ValueAt(primaryInput, thisInputDimensionOffset);
                auto outputValue = this->GetFunction().Compile(function, primaryValue, secondaryValues);
                function.SetValueAt(output, thisOutputDimensionOffset, outputValue);
            }
        });
    }

    template <typename ValueType, typename FunctionType>
    bool BroadcastFunctionNode<ValueType, FunctionType>::IsSecondaryInputPresent(int index) const
    {
        auto secondaryInput = GetSecondaryInput(index);
        if (secondaryInput)
        {
            return secondaryInput->Size() > 0;
        }
        else
        {
            return false;
        }
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastFunctionNode<ValueType, FunctionType>::Compute() const
    {
        auto outputSize = GetOutputMemoryLayout().GetExtent().NumElements();
        auto output = std::vector<ValueType>(outputSize);

        const size_t prevInputOffset = 0;
        const size_t prevOutputOffset = 0;
        std::vector<ValueType> secondaryValues(NumSecondaryInputs(), static_cast<ValueType>(0));
        ComputeDimensionLoop(0, output, prevInputOffset, prevOutputOffset, secondaryValues);

        GetOutput().SetOutput(output);
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastFunctionNode<ValueType, FunctionType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        const auto& compilerSettings = compiler.GetCompilerOptions();

        auto& module = function.GetModule();
        auto& emitter = module.GetIREmitter();
        auto valueType = emitter.Type(emitters::GetVariableType<ValueType>());
        auto valuePtrType = valueType->getPointerTo();

        const auto& primaryInput = GetPrimaryInput();
        auto primaryInputSize = primaryInput.Size();
        auto&& inputLayout = GetInputMemoryLayout();
        auto&& inputSize = inputLayout.GetActiveSize();
        auto secondaryInputSize = GetSecondaryInputSize();
        DEBUG_USED(secondaryInputSize);
        assert(secondaryInputSize == 0 || primaryInputSize % secondaryInputSize == 0);

        emitters::LLVMValue pPrimaryInput = compiler.EnsurePortEmitted(primaryInput);
        std::vector<emitters::LLVMValue> secondaryInputs;
        std::vector<emitters::LLVMValue> secondaryValues;
        for (int index = 0; index < NumSecondaryInputs(); ++index)
        {
            auto secondaryInputPort = GetSecondaryInput(index);
            auto secondaryInputSize = secondaryInputPort->Size();
            emitters::LLVMValue secondaryInput = (secondaryInputSize > 0) ? compiler.EnsurePortEmitted(*secondaryInputPort) : function.NullPointer(valuePtrType);
            secondaryInputs.push_back(secondaryInput);
            secondaryValues.push_back(nullptr);
        }
        emitters::LLVMValue pOutput = compiler.EnsurePortEmitted(GetOutput(), this->GetOutputPadding());

        // Call recursive function to emit nested loops
        // Note: We could just offset the input pointer at beginning instead of adding offset every time through the loop
        // Note: We can potentially fuse adjacent loops if memory is contiguous --- it can be done by preprocessing size/stride vectors
        bool allSecondaryInputsValid = true;
        for (int index = 0; index < NumSecondaryInputs(); ++index)
        {
            if (!IsSecondaryInputPresent(index))
            {
                allSecondaryInputsValid = false;
            }
        }

        const int minimumTaskSize = 4000;
        if (compilerSettings.parallelize && allSecondaryInputsValid && primaryInputSize > 2 * minimumTaskSize)
        {
            // computes ceil(a/b)
            auto CeilDiv = [](int a, int b) {
                return (a - 1) / b + 1;
            };

            // TODO: fix up logic for deciding how many tasks to use.
            //   want to specify minimum amount of work per task, and create fewer tasks
            //   if we don't have enough work.
            auto numOuterIterations = inputSize[0];
            const int numDesiredTasks = compilerSettings.maxThreads;
            int taskSize = std::max(CeilDiv(primaryInputSize, numDesiredTasks), minimumTaskSize);
            const int numTasks = std::min(CeilDiv(primaryInputSize, taskSize), compilerSettings.maxThreads);
            taskSize = CeilDiv(numOuterIterations, numTasks);

            // Ugly type-getting code to get around the type of the emitted port variables being different depending
            // on whether the node is inlined (or something).
            emitters::LLVMTypeList taskFunctionArgTypes{ pPrimaryInput->getType() };
            for (auto& secondaryInput : secondaryInputs)
            {
                taskFunctionArgTypes.push_back(secondaryInput->getType());
            }
            taskFunctionArgTypes.push_back(pOutput->getType());

            auto taskFunction = this->GetTaskFunction(compiler, function, taskFunctionArgTypes);
            std::vector<std::vector<emitters::LLVMValue>> taskArgs;
            for (int taskIndex = 0; taskIndex < numTasks; ++taskIndex)
            {
                auto begin = function.Literal<int>(taskIndex * taskSize);
                auto end = function.Literal<int>(std::min((taskIndex + 1) * taskSize, numOuterIterations));

                std::vector<emitters::LLVMValue> args{ pPrimaryInput };
                args.insert(args.end(), secondaryInputs.begin(), secondaryInputs.end());
                args.insert(args.end(), { pOutput, begin, end });
            }
            auto tasks = function.StartTasks(taskFunction, taskArgs);
            tasks.WaitAll(function);
        }
        else
        {
            auto prevInputDimensionOffset = function.LocalScalar();
            auto prevOutputDimensionOffset = function.LocalScalar();
            auto begin = function.LocalScalar<int>(0);
            auto end = function.LocalScalar<int>(inputSize[0]);
            EmitComputeDimensionLoop(compiler, function, 0, begin, end, pPrimaryInput, secondaryInputs, pOutput, prevInputDimensionOffset, prevOutputDimensionOffset, secondaryValues);
        }
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastFunctionNode<ValueType, FunctionType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        model::CompilableNode::WriteToArchive(archiver);

        archiver["inputLayout"] << _inputLayout;
        archiver["outputLayout"] << GetOutputMemoryLayout();
        archiver["broadcastDimension"] << _broadcastDimension;
        archiver["paddingValue"] << _paddingValue;
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastFunctionNode<ValueType, FunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        model::CompilableNode::ReadFromArchive(archiver);

        archiver["inputLayout"] >> _inputLayout;
        model::PortMemoryLayout outputLayout;
        archiver["outputLayout"] >> outputLayout;
        auto outputs = GetOutputPorts();
        for (auto p : outputs)
        {
            p->SetMemoryLayout(outputLayout);
        }
        archiver["broadcastDimension"] >> _broadcastDimension;
        archiver["paddingValue"] >> _paddingValue;
    }

    //
    // BroadcastUnaryFunctionNode
    //
    template <typename ValueType, typename FunctionType>
    BroadcastUnaryFunctionNode<ValueType, FunctionType>::BroadcastUnaryFunctionNode() :
        BroadcastFunctionNode<ValueType, FunctionType>({ &_primaryInput }, { &_output }),
        _primaryInput(this, {}, primaryInputPortName),
        _output(this, ell::model::Node::defaultOutputPortName, 0)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastUnaryFunctionNode<ValueType, FunctionType>::BroadcastUnaryFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout, const model::PortMemoryLayout& outputLayout, ValueType paddingValue) :
        BroadcastUnaryFunctionNode<ValueType, FunctionType>(primaryInput, inputLayout, outputLayout, FunctionType{}, paddingValue)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastUnaryFunctionNode<ValueType, FunctionType>::BroadcastUnaryFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout, const model::PortMemoryLayout& outputLayout, FunctionType function, ValueType paddingValue) :
        BroadcastFunctionNode<ValueType, FunctionType>({ &_primaryInput }, inputLayout, 0, { &_output }, outputLayout, function, paddingValue),
        _primaryInput(this, primaryInput, primaryInputPortName),
        _output(this, ell::model::Node::defaultOutputPortName, outputLayout)
    {
        // Verify sizes are compatible
        size_t totalInputSize = inputLayout.GetMemorySize();
        if (primaryInput.Size() < totalInputSize)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Primary input too small");
        }
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastUnaryFunctionNode<ValueType, FunctionType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& primaryInputElements = transformer.GetCorrespondingInputs(_primaryInput);
        auto broadcastFunction = GetFunction();
        auto newNode = transformer.AddNode<BroadcastUnaryFunctionNode<ValueType, FunctionType>>(primaryInputElements,
                                                                                                this->GetInputMemoryLayout(),
                                                                                                this->GetOutputMemoryLayout(),
                                                                                                broadcastFunction);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, typename FunctionType>
    utilities::ArchiveVersion BroadcastUnaryFunctionNode<ValueType, FunctionType>::GetArchiveVersion() const
    {
        constexpr utilities::ArchiveVersion archiveVersion = { utilities::ArchiveVersionNumbers::v5_refined_nodes };

        return archiveVersion;
    }

    template <typename ValueType, typename FunctionType>
    bool BroadcastUnaryFunctionNode<ValueType, FunctionType>::CanReadArchiveVersion(const utilities::ArchiveVersion& version) const
    {
        constexpr utilities::ArchiveVersion archiveVersion = { utilities::ArchiveVersionNumbers::v5_refined_nodes };

        return version >= archiveVersion;
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastUnaryFunctionNode<ValueType, FunctionType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        BroadcastFunctionNode<ValueType, FunctionType>::WriteToArchive(archiver);
        archiver[primaryInputPortName] << _primaryInput;
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastUnaryFunctionNode<ValueType, FunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        BroadcastFunctionNode<ValueType, FunctionType>::ReadFromArchive(archiver);
        archiver[primaryInputPortName] >> _primaryInput;
    }

    template <typename ValueType, typename FunctionType>
    const model::InputPort<ValueType>* BroadcastUnaryFunctionNode<ValueType, FunctionType>::GetSecondaryInput(int index) const
    {
        assert(index == 0);
        return nullptr;
    }

    //
    // BroadcastBinaryFunctionNode
    //
    template <typename ValueType, typename FunctionType>
    BroadcastBinaryFunctionNode<ValueType, FunctionType>::BroadcastBinaryFunctionNode() :
        BroadcastFunctionNode<ValueType, FunctionType>({ &_primaryInput, &_secondaryInput }, { &_output }),
        _primaryInput(this, {}, primaryInputPortName),
        _secondaryInput(this, {}, secondaryInputPortName),
        _output(this, ell::model::Node::defaultOutputPortName, 0)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastBinaryFunctionNode<ValueType, FunctionType>::BroadcastBinaryFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout, const model::OutputPort<ValueType>& secondaryInput, size_t dimension, const model::PortMemoryLayout& outputLayout, ValueType paddingValue) :
        BroadcastBinaryFunctionNode<ValueType, FunctionType>(primaryInput, inputLayout, secondaryInput, dimension, outputLayout, FunctionType{}, paddingValue)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastBinaryFunctionNode<ValueType, FunctionType>::BroadcastBinaryFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout, const model::OutputPort<ValueType>& secondaryInput, size_t dimension, const model::PortMemoryLayout& outputLayout, FunctionType function, ValueType paddingValue) :
        BroadcastFunctionNode<ValueType, FunctionType>({ &_primaryInput, &_secondaryInput }, inputLayout, dimension, { &_output }, outputLayout, function, paddingValue),
        _primaryInput(this, primaryInput, primaryInputPortName),
        _secondaryInput(this, secondaryInput, secondaryInputPortName),
        _output(this, ell::model::Node::defaultOutputPortName, outputLayout)
    {
        // Verify sizes are compatible
        size_t totalInputSize = inputLayout.GetMemorySize();
        if (primaryInput.Size() < totalInputSize)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Primary input too small");
        }

        if (secondaryInput.Size() != inputLayout.GetActiveSize(dimension))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Broadcast vector size doesn't match input");
        }
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastBinaryFunctionNode<ValueType, FunctionType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& primaryInputElements = transformer.GetCorrespondingInputs(_primaryInput);
        const auto& secondaryInputElements = transformer.GetCorrespondingInputs(_secondaryInput);
        auto newNode = transformer.AddNode<BroadcastBinaryFunctionNode<ValueType, FunctionType>>(primaryInputElements,
                                                                                                 this->GetInputMemoryLayout(),
                                                                                                 secondaryInputElements,
                                                                                                 this->GetBroadcastDimension(),
                                                                                                 this->GetOutputMemoryLayout(),
                                                                                                 GetFunction());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastBinaryFunctionNode<ValueType, FunctionType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        BroadcastFunctionNode<ValueType, FunctionType>::WriteToArchive(archiver);
        archiver[primaryInputPortName] << _primaryInput;
        archiver[secondaryInputPortName] << _secondaryInput;
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastBinaryFunctionNode<ValueType, FunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        BroadcastFunctionNode<ValueType, FunctionType>::ReadFromArchive(archiver);
        archiver[primaryInputPortName] >> _primaryInput;
        archiver[secondaryInputPortName] >> _secondaryInput;
    }

    template <typename ValueType, typename FunctionType>
    const model::InputPort<ValueType>* BroadcastBinaryFunctionNode<ValueType, FunctionType>::GetSecondaryInput(int index) const
    {
        assert(index == 0);
        return &_secondaryInput;
    }

    //
    // BroadcastTernaryFunctionNode
    //
    template <typename ValueType, typename FunctionType>
    BroadcastTernaryFunctionNode<ValueType, FunctionType>::BroadcastTernaryFunctionNode() :
        BroadcastFunctionNode<ValueType, FunctionType>({ &_primaryInput, &_secondaryInput1, &_secondaryInput2 }, { &_output }),
        _primaryInput(this, {}, primaryInputPortName),
        _secondaryInput1(this, {}, secondaryInput1PortName),
        _secondaryInput2(this, {}, secondaryInput2PortName),
        _output(this, ell::model::Node::defaultOutputPortName, 0)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastTernaryFunctionNode<ValueType, FunctionType>::BroadcastTernaryFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout, const model::OutputPort<ValueType>& secondaryInput1, const model::OutputPort<ValueType>& secondaryInput2, size_t dimension, const model::PortMemoryLayout& outputLayout, ValueType paddingValue) :
        BroadcastTernaryFunctionNode<ValueType, FunctionType>(primaryInput, inputLayout, secondaryInput1, secondaryInput2, dimension, outputLayout, FunctionType{}, paddingValue)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastTernaryFunctionNode<ValueType, FunctionType>::BroadcastTernaryFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout, const model::OutputPort<ValueType>& secondaryInput1, const model::OutputPort<ValueType>& secondaryInput2, size_t dimension, const model::PortMemoryLayout& outputLayout, FunctionType function, ValueType paddingValue) :
        BroadcastFunctionNode<ValueType, FunctionType>({ &_primaryInput, &_secondaryInput1, &_secondaryInput2 }, inputLayout, dimension, { &_output }, outputLayout, function, paddingValue),
        _primaryInput(this, primaryInput, primaryInputPortName),
        _secondaryInput1(this, secondaryInput1, secondaryInput1PortName),
        _secondaryInput2(this, secondaryInput2, secondaryInput2PortName),
        _output(this, ell::model::Node::defaultOutputPortName, outputLayout)
    {
        // Verify sizes are compatible
        size_t totalInputSize = inputLayout.GetMemorySize();
        if (primaryInput.Size() < totalInputSize)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Primary input too small");
        }

        if (std::max(secondaryInput1.Size(), secondaryInput2.Size()) != static_cast<size_t>(inputLayout.GetActiveSize(dimension)))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, std::string("Broadcast vector size doesn't match input, max(") + std::to_string(secondaryInput1.Size()) + ", " + std::to_string(secondaryInput2.Size()) + ") != " + std::to_string(inputLayout.GetActiveSize(dimension)));
        }

        if (secondaryInput1.Size() != secondaryInput2.Size() && secondaryInput1.Size() > 0 && secondaryInput2.Size() > 0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "If present, secondary inputs must have the same size");
        }

        if (inputLayout.GetActiveSize() != outputLayout.GetActiveSize())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "BroadcastFunctionNode: Input and output active area sizes don't match");
        }
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastTernaryFunctionNode<ValueType, FunctionType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& primaryInputElements = transformer.GetCorrespondingInputs(_primaryInput);
        const auto& secondaryInput1Elements = transformer.GetCorrespondingInputs(_secondaryInput1);
        const auto& secondaryInput2Elements = transformer.GetCorrespondingInputs(_secondaryInput2);
        auto newNode = transformer.AddNode<BroadcastTernaryFunctionNode<ValueType, FunctionType>>(primaryInputElements,
                                                                                                  this->GetInputMemoryLayout(),
                                                                                                  secondaryInput1Elements,
                                                                                                  secondaryInput2Elements,
                                                                                                  this->GetBroadcastDimension(),
                                                                                                  this->GetOutputMemoryLayout(),
                                                                                                  GetFunction());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastTernaryFunctionNode<ValueType, FunctionType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        BroadcastFunctionNode<ValueType, FunctionType>::WriteToArchive(archiver);
        archiver[primaryInputPortName] << _primaryInput;
        archiver[secondaryInput1PortName] << _secondaryInput1;
        archiver[secondaryInput2PortName] << _secondaryInput2;
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastTernaryFunctionNode<ValueType, FunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        BroadcastFunctionNode<ValueType, FunctionType>::ReadFromArchive(archiver);
        archiver[primaryInputPortName] >> _primaryInput;
        archiver[secondaryInput1PortName] >> _secondaryInput1;
        archiver[secondaryInput2PortName] >> _secondaryInput2;
    }

    template <typename ValueType, typename FunctionType>
    const model::InputPort<ValueType>* BroadcastTernaryFunctionNode<ValueType, FunctionType>::GetSecondaryInput(int index) const
    {
        assert(index < 2);
        if (index == 0)
        {
            return &secondaryInput1;
        }
        else if (index == 1)
        {
            return &secondaryInput2;
        }
        return nullptr;
    }

    //
    // BroadcastLinearFunctionNode
    //
    template <typename ValueType>
    BroadcastLinearFunctionNode<ValueType>::BroadcastLinearFunctionNode() :
        BroadcastTernaryFunctionNode<ValueType, BroadcastLinearFunction<ValueType>>()
    {
    }

    template <typename ValueType>
    BroadcastLinearFunctionNode<ValueType>::BroadcastLinearFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout, const model::OutputPort<ValueType>& scaleInput, const model::OutputPort<ValueType>& biasInput, size_t dimension, const model::PortMemoryLayout& outputLayout, ValueType paddingValue) :
        BroadcastTernaryFunctionNode<ValueType, BroadcastLinearFunction<ValueType>>(primaryInput, inputLayout, scaleInput, biasInput, dimension, outputLayout, paddingValue)
    {
    }

    template <typename ValueType>
    void BroadcastLinearFunctionNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& primaryInputElements = transformer.GetCorrespondingInputs(primaryInput);
        const auto& scaleInputElements = transformer.GetCorrespondingInputs(secondaryInput1);
        const auto& biasInputElements = transformer.GetCorrespondingInputs(secondaryInput2);
        auto newNode = transformer.AddNode<BroadcastLinearFunctionNode<ValueType>>(primaryInputElements,
                                                                                   this->GetInputMemoryLayout(),
                                                                                   scaleInputElements,
                                                                                   biasInputElements,
                                                                                   this->GetBroadcastDimension(),
                                                                                   this->GetOutputMemoryLayout());
        transformer.MapNodeOutput(output, newNode->output);
    }

} // namespace nodes
} // namespace ell

#pragma endregion implementation
