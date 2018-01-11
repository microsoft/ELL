////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IIRFilterNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IIRFilterNode.h"

// emitters
#include "EmitterException.h"
#include "EmitterTypes.h"
#include "IRLocalValue.h"

// math
#include "FFT.h"
#include "MathConstants.h"

#include <iostream>

namespace ell
{
namespace nodes
{
    namespace detail
    {
        // Returns an array with the coeff values in reverse order, and almost doubled in length
        // [b0, b1, b2] is returned as [b2, b1, b0, b2, b1, b0]
        template <typename ValueType>
        std::vector<ValueType> GetFilterCoeffArray(const std::vector<ValueType>& coeffs)
        {
            std::vector<ValueType> result;
            result.reserve(coeffs.size() * 2);
            std::reverse_copy(coeffs.begin(), coeffs.end(), std::back_inserter(result));
            std::reverse_copy(coeffs.begin(), coeffs.end(), std::back_inserter(result));

            // This is slightly wasteful of memory --- we only need the last entry in this array
            // to make the indexing simpler for the recurrent filter coefficients
            // The non-recurrent coefficients could use this and be fine:
            // std::reverse_copy(coeffs.begin()+1, coeffs.end(), std::back_inserter(result));
            return result;
        }
    }

    template <typename ValueType>
    IIRFilterNode<ValueType>::IIRFilterNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0), _filter({}, {})
    {
    }

    template <typename ValueType>
    IIRFilterNode<ValueType>::IIRFilterNode(const model::PortElements<ValueType>& input, const std::vector<ValueType>& b, const std::vector<ValueType>& a)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, _input.Size()), _filter(b, a)
    {
    }

    template <typename ValueType>
    void IIRFilterNode<ValueType>::Compute() const
    {
        std::vector<ValueType> output = _filter.FilterSamples(_input.GetValue());
        _output.SetOutput(output);
    };

    template <typename ValueType>
    void IIRFilterNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<IIRFilterNode<ValueType>>(newPortElements, _filter.GetFeedforwardCoefficients(), _filter.GetRecursiveCoefficients());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void IIRFilterNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        using namespace std::string_literals;

        auto& module = function.GetModule();
        const auto inputSize = input.Size();
        const auto bSize = _filter.GetFeedforwardCoefficients().size();
        const auto aSize = _filter.GetRecursiveCoefficients().size();

        // Allocate global variables to accumulate the previous input and output
        llvm::GlobalVariable* prevInput = module.GlobalArray("prevInput_"s + GetInternalStateIdentifier(), std::vector<ValueType>(bSize, 0.0));
        llvm::GlobalVariable* prevOutput = module.GlobalArray("prevOutput_"s + GetInternalStateIdentifier(), std::vector<ValueType>(aSize, 0.0));

        // Allocate global constants for the A and B filter coefficients
        std::vector<ValueType> bCoeffValues = detail::GetFilterCoeffArray(_filter.GetFeedforwardCoefficients());
        std::vector<ValueType> aCoeffValues = detail::GetFilterCoeffArray(_filter.GetRecursiveCoefficients());
        llvm::GlobalVariable* bCoeffs = module.ConstantArray("bCoeffs_"s + GetInternalStateIdentifier(), bCoeffValues);
        llvm::GlobalVariable* aCoeffs = module.ConstantArray("aCoeffs_"s + GetInternalStateIdentifier(), aCoeffValues);

        // Allocate global variable for current input and output indices
        llvm::GlobalVariable* xIndexVar = module.Global<int>("xIndex_"s + GetInternalStateIdentifier(), 0);
        llvm::GlobalVariable* yIndexVar = module.Global<int>("yIndex_"s + GetInternalStateIdentifier(), 0);

        // Get input
        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        // Allocate local variable to accumulate y
        llvm::Value* yVar = function.Variable(emitters::GetVariableType<ValueType>(), "y");

        // Loop over input entries
        function.For(inputSize, [=](emitters::IRFunctionEmitter& function, llvm::Value* inputIndex)
        {
            llvm::Value* inputVal = nullptr;
            if (inputSize == 1) // scalar input
            {
                inputVal = pInput;
            }
            else
            {
                inputVal = function.ValueAt(pInput, inputIndex);
            }

            // zero out accumulator
            function.StoreZero(yVar);

            // Store current x_t
            auto xIndex = function.LocalScalar(function.Load(xIndexVar));
            function.SetValueAt(prevInput, xIndex, inputVal); // X[i] = x_t

            // compute dot product X dot B
            auto bOffset = function.LocalScalar((int)bSize - 1) - xIndex;
            function.For(bSize, [prevInput, bCoeffs, bOffset, yVar](emitters::IRFunctionEmitter& function, llvm::Value* iVar) 
            {
                auto i = function.LocalScalar(iVar);
                auto xVal = function.LocalScalar(function.ValueAt(prevInput, i));
                auto bVal = function.LocalScalar(function.ValueAt(bCoeffs, bOffset + i));
                function.Store(yVar, function.LocalScalar(function.Load(yVar)) + (xVal * bVal));
            });

            // compute dot product Y dot A
            auto yIndex = function.LocalScalar(function.Load(yIndexVar));
            auto aOffset = function.LocalScalar((int)aSize) - yIndex;
            function.For(aSize, [prevOutput, aCoeffs, aOffset, yVar](emitters::IRFunctionEmitter& function, llvm::Value* jVar) 
            {
                auto j = function.LocalScalar(jVar);
                auto yVal = function.LocalScalar(function.ValueAt(prevOutput, j));
                auto aVal = function.LocalScalar(function.ValueAt(aCoeffs, aOffset + j));
                function.Store(yVar, function.LocalScalar(function.Load(yVar)) - (yVal * aVal));
            });

            // Store current y_t
            auto y = function.Load(yVar);
            function.SetValueAt(prevOutput, yIndex, y); // Y[i] = y_t
            function.SetValueAt(pOutput, inputIndex, y); // output[inputIndex] = y_t

            // Update xIndex and yIndex
            function.Store(xIndexVar, function.Operator(emitters::TypedOperator::moduloSigned, xIndex + function.LocalScalar(1), function.LocalScalar((int)bSize)));
            function.Store(yIndexVar, function.Operator(emitters::TypedOperator::moduloSigned, yIndex + function.LocalScalar(1), function.LocalScalar((int)aSize)));
        });
    }

    template <typename ValueType>
    void IIRFilterNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["filter"] << _filter;
    }

    template <typename ValueType>
    void IIRFilterNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["filter"] >> _filter;
        _output.SetSize(_input.Size());
    }

    //
    // Explicit instantiation definitions
    //
    template class IIRFilterNode<float>;
    template class IIRFilterNode<double>;
} // nodes
} // ell
