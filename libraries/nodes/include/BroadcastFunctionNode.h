////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BroadcastFunctionNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNode.h"
#include "CompilableNodeUtilities.h"
#include "IRMapCompiler.h"
#include "Model.h"
#include "ModelTransformer.h"
#include "Node.h"

// nodes
#include "ConstantNode.h"
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
        virtual ~BroadcastUnaryFunction() {}

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
        virtual ~BroadcastBinaryFunction() {}

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
        virtual ~BroadcastTernaryFunction() {}

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
        virtual ValueType Compute(ValueType x, ValueType a, ValueType b) const override;
        using BroadcastTernaryFunction<ValueType>::Compute;

        /// <summary> Emits IR to compute a value </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="a"> The first secondary value </param>
        /// <param name="b"> The second secondary value </param>
        /// <returns> The value the function f(x,a,b) = ax + b </returns>
        virtual llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x, llvm::Value* a, llvm::Value* b) const override;
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

    protected:
        BroadcastFunctionNode(const std::vector<model::InputPortBase*>& inputs, const std::vector<model::OutputPortBase*>& outputs);

        BroadcastFunctionNode(const std::vector<model::InputPortBase*>& inputs,
                              const PortMemoryLayout& inputLayout, size_t broadcastDimension,
                              const std::vector<model::OutputPortBase*>& outputs,
                              const PortMemoryLayout& outputLayout,
                              FunctionType function,
                              ValueType padding = 0);

        const PortMemoryLayout& GetInputLayout() const { return _inputLayout; }
        const PortMemoryLayout& GetOutputLayout() const { return _outputLayout; }
        size_t GetBroadcastDimension() const { return _broadcastDimension; }
        size_t NumPrimaryInputDimensions() const { return _inputLayout.size.size(); }

        static size_t NumElements(const Shape& size);
        virtual bool HasState() const override { return true; }

        virtual const model::InputPort<ValueType>& GetPrimaryInput() const = 0;
        virtual const model::InputPort<ValueType>* GetSecondaryInput(int index) const = 0;
        FunctionType GetFunction() const { return _function; }

        // Helpers for generating nested loops to visit all input/output values
        void ComputeDimensionLoop(size_t dimension, std::vector<ValueType>& output, size_t prevInputDimensionOffset, size_t prevOutputDimensionOffset, std::vector<ValueType>& secondaryValues) const;
        void EmitComputeDimensionLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, size_t dimension, llvm::Value* primaryInput, const std::vector<llvm::Value*>& secondaryInputs, llvm::Value* output, llvm::Value* prevInputDimensionOffset, llvm::Value* prevOutputDimensionOffset, std::vector<llvm::Value*>& secondaryValues) const;

        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        ValueType GetOutputPadding() const { return _paddingValue; }

    private:
        PortMemoryLayout _inputLayout;
        PortMemoryLayout _outputLayout;
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
        static constexpr const char* outputPortName = "output";
        const model::InputPort<ValueType>& primaryInput = _primaryInput;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        BroadcastUnaryFunctionNode();

        BroadcastUnaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const PortMemoryLayout& inputLayout,
                                   const PortMemoryLayout& outputLayout,
                                   ValueType padding = 0);

        BroadcastUnaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const PortMemoryLayout& inputLayout,
                                   const PortMemoryLayout& outputLayout,
                                   FunctionType function,
                                   ValueType padding = 0);

        virtual int GetPrimaryInputSize() const override { return _primaryInput.Size(); }
        virtual int GetSecondaryInputSize() const override { return 0; }
        virtual int NumSecondaryInputs() const override { return 0; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BroadcastUnaryFunctionNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        using BroadcastFunctionNode<ValueType, FunctionType>::GetInputLayout;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetOutputLayout;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetBroadcastDimension;
        using BroadcastFunctionNode<ValueType, FunctionType>::NumPrimaryInputDimensions;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetFunction;
        using BroadcastFunctionNode<ValueType, FunctionType>::NumElements;

        virtual void Copy(model::ModelTransformer& transformer) const override;
        virtual void Compute() const override;
        virtual void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        virtual const model::InputPort<ValueType>& GetPrimaryInput() const override { return primaryInput; }
        virtual const model::InputPort<ValueType>* GetSecondaryInput(int index) const override;

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
        static constexpr const char* outputPortName = "output";
        const model::InputPort<ValueType>& primaryInput = _primaryInput;
        const model::InputPort<ValueType>& secondaryInput = _secondaryInput;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        BroadcastBinaryFunctionNode();
        BroadcastBinaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const PortMemoryLayout& inputLayout,
                                    const model::PortElements<ValueType>& secondaryInput, size_t secondaryInputDimension,
                                    const PortMemoryLayout& outputLayout,
                                    ValueType padding = 0);
        BroadcastBinaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const PortMemoryLayout& inputLayout,
                                    const model::PortElements<ValueType>& secondaryInput, size_t secondaryInputDimension,
                                    const PortMemoryLayout& outputLayout,
                                    FunctionType function,
                                    ValueType padding = 0);

        virtual int GetPrimaryInputSize() const override { return _primaryInput.Size(); }
        virtual int GetSecondaryInputSize() const override { return _secondaryInput.Size(); }
        virtual int NumSecondaryInputs() const override { return 1; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BroadcastBinaryFunctionNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        using BroadcastFunctionNode<ValueType, FunctionType>::GetInputLayout;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetOutputLayout;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetBroadcastDimension;
        using BroadcastFunctionNode<ValueType, FunctionType>::NumPrimaryInputDimensions;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetFunction;
        using BroadcastFunctionNode<ValueType, FunctionType>::NumElements;

        virtual void Copy(model::ModelTransformer& transformer) const override;
        virtual void Compute() const override;
        virtual void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        virtual const model::InputPort<ValueType>& GetPrimaryInput() const override { return primaryInput; }
        virtual const model::InputPort<ValueType>* GetSecondaryInput(int index) const override;

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
        static constexpr const char* outputPortName = "output";
        const model::InputPort<ValueType>& primaryInput = _primaryInput;
        const model::InputPort<ValueType>& secondaryInput1 = _secondaryInput1;
        const model::InputPort<ValueType>& secondaryInput2 = _secondaryInput2;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        BroadcastTernaryFunctionNode();
        BroadcastTernaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const PortMemoryLayout& inputLayout,
                                     const model::PortElements<ValueType>& secondaryInput1, const model::PortElements<ValueType>& secondaryInput2, size_t secondaryInputDimension,
                                     const PortMemoryLayout& outputLayout,
                                     ValueType padding = 0);
        BroadcastTernaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const PortMemoryLayout& inputLayout,
                                     const model::PortElements<ValueType>& secondaryInput1, const model::PortElements<ValueType>& secondaryInput2, size_t secondaryInputDimension,
                                     const PortMemoryLayout& outputLayout,
                                     FunctionType function,
                                     ValueType padding = 0);

        virtual int GetPrimaryInputSize() const override { return _primaryInput.Size(); }
        virtual int GetSecondaryInputSize() const override { return std::max(_secondaryInput1.Size(), _secondaryInput2.Size()); }
        virtual int NumSecondaryInputs() const override { return 2; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BroadcastTernaryFunctionNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        using BroadcastFunctionNode<ValueType, FunctionType>::GetInputLayout;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetOutputLayout;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetBroadcastDimension;
        using BroadcastFunctionNode<ValueType, FunctionType>::NumPrimaryInputDimensions;
        using BroadcastFunctionNode<ValueType, FunctionType>::NumElements;
        using BroadcastFunctionNode<ValueType, FunctionType>::GetFunction;

        virtual void Copy(model::ModelTransformer& transformer) const override;
        virtual void Compute() const override;
        virtual void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        virtual const model::InputPort<ValueType>& GetPrimaryInput() const override { return primaryInput; }
        virtual const model::InputPort<ValueType>* GetSecondaryInput(int index) const override;

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
    // Special case of BroadcastTernaryFuncitonNode, using a linear function
    //
    template <typename ValueType>
    class BroadcastLinearFunctionNode : public BroadcastTernaryFunctionNode<ValueType, BroadcastLinearFunction<ValueType>>
    {
    public:
        using BroadcastTernaryFunctionNode<ValueType, BroadcastLinearFunction<ValueType>>::primaryInput;
        using BroadcastTernaryFunctionNode<ValueType, BroadcastLinearFunction<ValueType>>::secondaryInput1;
        using BroadcastTernaryFunctionNode<ValueType, BroadcastLinearFunction<ValueType>>::secondaryInput2;
        using BroadcastTernaryFunctionNode<ValueType, BroadcastLinearFunction<ValueType>>::output;

        BroadcastLinearFunctionNode();
        BroadcastLinearFunctionNode(const model::PortElements<ValueType>& primaryInput, const PortMemoryLayout& inputLayout,
                                    const model::PortElements<ValueType>& scaleInput, const model::PortElements<ValueType>& biasInput, size_t secondaryInputDimension,
                                    const PortMemoryLayout& outputLayout,
                                    ValueType padding = 0);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BroadcastLinearFunctionNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        virtual bool IsCompilable() const override;

    protected:
        virtual void Copy(model::ModelTransformer& transformer) const override;
        virtual bool Refine(model::ModelTransformer& transformer) const override;
        virtual bool HasState() const override { return false; }
        bool HasScale() const { return secondaryInput1.Size() != 0; }
        bool HasBias() const { return secondaryInput2.Size() != 0; }

        // Helpers to fold sequential linear operations together
        bool HasSimpleConstantSecondaryInputs() const;
        bool CanCombineWithPrimaryInput() const;
        struct LinearCoeffNodes
        {
            const ConstantNode<ValueType>* scaleNode;
            const ConstantNode<ValueType>* biasNode;
        };
        LinearCoeffNodes GetConstantSecondaryInputNodes() const;
        void GetCombinedLinearCoeffs(const BroadcastLinearFunctionNode<ValueType>& prevNode, std::vector<ValueType>& scale, std::vector<ValueType>& bias) const;
    };
}
}

#include "../tcc/BroadcastFunctionNode.tcc"
