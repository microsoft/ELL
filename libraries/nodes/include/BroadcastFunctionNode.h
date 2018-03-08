////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BroadcastFunctionNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ConstantNode.h"

// emitters
#include "IRAsyncTask.h"
#include "IREmitter.h"
#include "IRVectorUtilities.h"
#include "LLVMUtilities.h"

// model
#include "CompilableNode.h"
#include "CompilableNodeUtilities.h"
#include "IRMapCompiler.h"
#include "Model.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "PortMemoryLayout.h"

// utilities
#include "Exception.h"
#include "TypeName.h"

// stl
#include <cassert>
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
        virtual llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x) const = 0;

        /// <summary> Emits IR to compute a value </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="secondaryArgs"> The secondary values. Must be empty. </param>
        /// <returns> The value the function f(x) </returns>
        llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x, const std::vector<llvm::Value*>& secondaryArgs) const;

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
        virtual llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x, llvm::Value* a) const = 0;

        /// <summary> Emits IR to compute a value </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="secondaryArgs"> The secondary values. Must have size 1. </param>
        /// <returns> The value the function f(x,a) </returns>
        llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x, const std::vector<llvm::Value*>& secondaryArgs) const;

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
        virtual llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x, llvm::Value* a, llvm::Value* b) const = 0;

        /// <summary> Emits IR to compute a value </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="secondaryArgs"> The secondary values. Must have size 2. </param>
        /// <returns> The value the function f(x,a) </returns>
        llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x, const std::vector<llvm::Value*>& secondaryArgs) const;

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
        llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x, llvm::Value* a, llvm::Value* b) const override;
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

        const model::PortMemoryLayout& GetInputLayout() const { return _inputLayout; }
        const model::PortMemoryLayout& GetOutputLayout() const { return _outputLayout; }

        size_t GetBroadcastDimension() const { return _broadcastDimension; }
        size_t NumPrimaryInputDimensions() const { return _inputLayout.NumDimensions(); }

    protected:
        BroadcastFunctionNode(const std::vector<model::InputPortBase*>& inputs, const std::vector<model::OutputPortBase*>& outputs);

        BroadcastFunctionNode(const std::vector<model::InputPortBase*>& inputs,
                              const model::PortMemoryLayout& inputLayout, size_t broadcastDimension,
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
        void EmitComputeDimensionLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, size_t dimension, llvm::Value* begin, llvm::Value* end, llvm::Value* primaryInput, const std::vector<llvm::Value*>& secondaryInputs, llvm::Value* output, llvm::Value* prevInputDimensionOffset, llvm::Value* prevOutputDimensionOffset, std::vector<llvm::Value*>& secondaryValues) const;
        emitters::IRFunctionEmitter GetTaskFunction(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, const emitters::LLVMTypeList& portTypes) const;

        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        ValueType GetOutputPadding() const { return _paddingValue; }

    private:
        model::PortMemoryLayout _inputLayout;
        model::PortMemoryLayout _outputLayout;
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
        BroadcastUnaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                   const model::PortMemoryLayout& outputLayout,
                                   ValueType padding = 0);

        /// <summary></summary>
        BroadcastUnaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                   const model::PortMemoryLayout& outputLayout,
                                   FunctionType function,
                                   ValueType padding = 0);

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

        using BroadcastFunctionNode<ValueType, FunctionType>::GetInputLayout;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetOutputLayout;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetBroadcastDimension;
        using BroadcastFunctionNode<ValueType, FunctionType>::NumPrimaryInputDimensions;

    protected:
        using BroadcastFunctionNode<ValueType, FunctionType>::GetFunction;

        void Copy(model::ModelTransformer& transformer) const override;
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
        BroadcastBinaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                    const model::PortElements<ValueType>& secondaryInput, size_t secondaryInputDimension,
                                    const model::PortMemoryLayout& outputLayout,
                                    ValueType padding = 0);

        /// <summary></summary>
        BroadcastBinaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                    const model::PortElements<ValueType>& secondaryInput, size_t secondaryInputDimension,
                                    const model::PortMemoryLayout& outputLayout,
                                    FunctionType function,
                                    ValueType padding = 0);

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

        using BroadcastFunctionNode<ValueType, FunctionType>::GetInputLayout;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetOutputLayout;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetBroadcastDimension;
        using BroadcastFunctionNode<ValueType, FunctionType>::NumPrimaryInputDimensions;

    protected:
        using BroadcastFunctionNode<ValueType, FunctionType>::GetFunction;
        using BroadcastFunctionNode<ValueType, FunctionType>::NumElements;

        void Copy(model::ModelTransformer& transformer) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        const model::InputPort<ValueType>& GetPrimaryInput() const override { return primaryInput; }
        const model::InputPort<ValueType>* GetSecondaryInput(int index) const override;
        const model::OutputPort<ValueType>& GetOutput() const override { return output; }

    private:
        using BroadcastFunctionNode<ValueType, FunctionType>::ComputeDimensionLoop;
        using BroadcastFunctionNode<ValueType, FunctionType>::EmitComputeDimensionLoop;

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
        BroadcastTernaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                     const model::PortElements<ValueType>& secondaryInput1, const model::PortElements<ValueType>& secondaryInput2, size_t secondaryInputDimension,
                                     const model::PortMemoryLayout& outputLayout,
                                     ValueType padding = 0);

        /// <summary></summary>
        BroadcastTernaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                     const model::PortElements<ValueType>& secondaryInput1, const model::PortElements<ValueType>& secondaryInput2, size_t secondaryInputDimension,
                                     const model::PortMemoryLayout& outputLayout,
                                     FunctionType function,
                                     ValueType padding = 0);

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

        using BroadcastFunctionNode<ValueType, FunctionType>::GetInputLayout;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetOutputLayout;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetBroadcastDimension;
        using BroadcastFunctionNode<ValueType, FunctionType>::NumPrimaryInputDimensions;

    protected:
        using BroadcastFunctionNode<ValueType, FunctionType>::GetFunction;

        void Copy(model::ModelTransformer& transformer) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        const model::InputPort<ValueType>& GetPrimaryInput() const override { return primaryInput; }
        const model::InputPort<ValueType>* GetSecondaryInput(int index) const override;
        const model::OutputPort<ValueType>& GetOutput() const override { return output; }

    private:
        using BroadcastFunctionNode<ValueType, FunctionType>::ComputeDimensionLoop;
        using BroadcastFunctionNode<ValueType, FunctionType>::EmitComputeDimensionLoop;

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
        BroadcastLinearFunctionNode(const model::PortElements<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                    const model::PortElements<ValueType>& scaleInput, const model::PortElements<ValueType>& biasInput, size_t secondaryInputDimension,
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
        void Copy(model::ModelTransformer& transformer) const override;
        bool HasState() const override { return false; }
        bool HasScale() const { return secondaryInput1.Size() != 0; }
        bool HasBias() const { return secondaryInput2.Size() != 0; }
    };
}
}

#include "../tcc/BroadcastFunctionNode.tcc"
