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
    /// </summary>
    class BroadcastFunctionNode;

    using Shape = const std::vector<size_t>;

    //
    // Base class for binary function types
    //
    template <typename ValueType>
    class BroadcastBinaryFunction
    {
    public:
        BroadcastBinaryFunction() = default;
        BroadcastBinaryFunction(const BroadcastBinaryFunction&) = default;

        /// <summary> Computes a value (on the host machine) </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="a"> The secondary value </param>
        /// <returns> The value the function f(x,a) </returns>
        ValueType Compute(ValueType x, ValueType a) const;

        /// <summary> Emits IR to compute a value </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="a"> The secondary value </param>
        /// <returns> The value the function f(x,a) </returns>
        llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x, llvm::Value* a) const;

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

        /// <summary> Computes a value (on the host machine) </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="a"> The first secondary value </param>
        /// <param name="b"> The second secondary value </param>
        /// <returns> The value the function f(x,a,b) </returns>
        ValueType Compute(ValueType x, ValueType a, ValueType b) const;

        /// <summary> Emits IR to compute a value </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="a"> The first secondary value </param>
        /// <param name="b"> The second secondary value </param>
        /// <returns> The value the function f(x,a,b) </returns>
        llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x, llvm::Value* a, llvm::Value* b) const;

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
        ValueType Compute(ValueType x, ValueType a, ValueType b) const;

        /// <summary> Emits IR to compute a value </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <param name="a"> The first secondary value </param>
        /// <param name="b"> The second secondary value </param>
        /// <returns> The value the function f(x,a,b) = ax + b </returns>
        llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x, llvm::Value* a, llvm::Value* b) const;

        /// <summary> Indicates if the function can operate on vector types </summary>
        bool CanUseVectorTypes() const { return true; }
    };

    //
    // Base class for broadcast nodes
    //
    class BroadcastFunctionNode : public model::CompilableNode
    {
    public:
        virtual int GetPrimaryInputSize() const = 0;
        virtual int GetSecondaryInputSize() const = 0;

    protected:
        BroadcastFunctionNode(const std::vector<model::InputPortBase*>& inputs, const std::vector<model::OutputPortBase*>& outputs);

        BroadcastFunctionNode(const std::vector<model::InputPortBase*>& inputs,
                              const Shape& inputStride, const Shape& inputOffset, const Shape& inputSize, size_t broadcastDimension,
                              const std::vector<model::OutputPortBase*>& outputs,
                              const Shape& outputStride, const Shape& outputOffset);

        const Shape& GetInputStride() const { return _inputStride; }
        const Shape& GetInputOffset() const { return _inputOffset; }
        const Shape& GetInputSize() const { return _inputSize; }
        const Shape& GetOutputStride() const { return _outputStride; }
        const Shape& GetOutputOffset() const { return _outputOffset; }
        size_t GetBroadcastDimension() const { return _broadcastDimension; }
        size_t NumPrimaryInputDimensions() const { return _inputSize.size(); }

        static size_t NumElements(const Shape& size);
        static bool ShapesEqual(const Shape& shape1, const Shape& shape2);
        virtual bool HasState() const override { return true; }

    private:
        // Size parameters
        Shape _inputStride;
        Shape _inputOffset;
        Shape _inputSize;
        Shape _outputStride;
        Shape _outputOffset;
        size_t _broadcastDimension = 0;
    };

    //
    // BroadcastBinaryFunctionNode
    //
    template <typename ValueType, typename FunctionType>
    class BroadcastBinaryFunctionNode : public BroadcastFunctionNode
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
        BroadcastBinaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const Shape& inputStride, const Shape& inputOffset, const Shape& inputSize,
                                     const model::PortElements<ValueType>& secondaryInput, size_t secondaryInputDimension,
                                     const Shape& outputStride, const Shape& outputOffset);
        BroadcastBinaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const Shape& inputStride, const Shape& inputOffset, const Shape& inputSize,
                                     const model::PortElements<ValueType>& secondaryInput, size_t secondaryInputDimension,
                                     const Shape& outputStride, const Shape& outputOffset,
                                     const FunctionType& function);

        virtual int GetPrimaryInputSize() const override { return _primaryInput.Size(); }
        virtual int GetSecondaryInputSize() const override { return _secondaryInput.Size(); }

        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BroadcastBinaryFunctionNode"); }
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        virtual void Copy(model::ModelTransformer& transformer) const override;
        virtual void Compute() const override;
        virtual void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        // TODO: replace with individual secondary value parameters with vector (or tuple) of secondary values
        void ComputeDimensionLoop(size_t dimension, std::vector<ValueType>& output, size_t prevInputDimensionOffset, size_t prevOutputDimensionOffset, ValueType secondaryValue) const;
        void EmitComputeDimensionLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, size_t dimension, llvm::Value* primaryInput, llvm::Value* secondaryInput, llvm::Value* output, llvm::Value* prevInputDimensionOffset, llvm::Value* prevOutputDimensionOffset, llvm::Value* secondaryValue) const;

        // Inputs
        model::InputPort<ValueType> _primaryInput;
        model::InputPort<ValueType> _secondaryInput;

        // Output
        model::OutputPort<ValueType> _output;

        FunctionType _function;
    };

    //
    // BroadcastTernaryFunctionNode
    //
    template <typename ValueType, typename FunctionType>
    class BroadcastTernaryFunctionNode : public BroadcastFunctionNode
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
        BroadcastTernaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const Shape& inputStride, const Shape& inputOffset, const Shape& inputSize,
                                     const model::PortElements<ValueType>& secondaryInput1, const model::PortElements<ValueType>& secondaryInput2, size_t secondaryInputDimension,
                                     const Shape& outputStride, const Shape& outputOffset);
        BroadcastTernaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const Shape& inputStride, const Shape& inputOffset, const Shape& inputSize,
                                     const model::PortElements<ValueType>& secondaryInput1, const model::PortElements<ValueType>& secondaryInput2, size_t secondaryInputDimension,
                                     const Shape& outputStride, const Shape& outputOffset,
                                     const FunctionType& function);

        virtual int GetPrimaryInputSize() const override { return _primaryInput.Size(); }
        virtual int GetSecondaryInputSize() const override { return std::max(_secondaryInput1.Size(), _secondaryInput2.Size()); }

        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BroadcastTernaryFunctionNode"); }
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        virtual void Copy(model::ModelTransformer& transformer) const override;
        virtual void Compute() const override;
        virtual void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        // TODO: replace with individual secondary value parameters with vector (or tuple!) of secondary values
        void ComputeDimensionLoop(size_t dimension, std::vector<ValueType>& output, size_t prevInputDimensionOffset, size_t prevOutputDimensionOffset, ValueType secondaryValue1, ValueType secondaryValue2) const;
        void EmitComputeDimensionLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, size_t dimension, llvm::Value* primaryInput, llvm::Value* secondaryInput1, llvm::Value* secondaryInput2, llvm::Value* output, llvm::Value* prevInputDimensionOffset, llvm::Value* prevOutputDimensionOffset, llvm::Value* secondaryValue1, llvm::Value* secondaryValue2) const;

        // Inputs
        model::InputPort<ValueType> _primaryInput;
        model::InputPort<ValueType> _secondaryInput1;
        model::InputPort<ValueType> _secondaryInput2;

        // Output
        model::OutputPort<ValueType> _output;

        FunctionType _function;
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
        BroadcastLinearFunctionNode(const model::PortElements<ValueType>& primaryInput, const Shape& inputStride, const Shape& inputOffset, const Shape& inputSize,
                                    const model::PortElements<ValueType>& scaleInput, const model::PortElements<ValueType>& biasInput, size_t secondaryInputDimension,
                                    const Shape& outputStride, const Shape& outputOffset);

        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BroadcastLinearFunctionNode"); }
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

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
