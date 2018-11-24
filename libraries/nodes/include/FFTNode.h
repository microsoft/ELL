////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FFTNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// emitters
#include "LLVMUtilities.h"

// model
#include "CompilableNode.h"
#include "IRMapCompiler.h"
#include "InputPort.h"
#include "MapCompiler.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "OutputPort.h"
#include "PortElements.h"

// utilities
#include "TypeName.h"
#include "TypeTraits.h"

// stl
#include <cmath>
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that performs a real-valued discrete ("fast") fourier transform (FFT) on its input </summary>
    template <typename ValueType>
    class FFTNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        FFTNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The signal to process. </param>
        FFTNode(const model::OutputPort<ValueType>& input);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("FFTNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return false; }

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Emitting IR for FFT implemenations
        void EmitFFT_2(emitters::IRFunctionEmitter& function, emitters::LLVMValue input);
        void EmitFFT_4(emitters::IRFunctionEmitter& function, emitters::LLVMValue input);
        void EmitFFT(emitters::IRFunctionEmitter& function, size_t length, emitters::LLVMValue input, emitters::LLVMValue scratch);
        void EmitRealFFT(emitters::IRFunctionEmitter& function, size_t length, emitters::LLVMValue input, emitters::LLVMValue scratch, emitters::LLVMValue complexInput);

        // Getting FFT functions
        emitters::LLVMFunction GetRealFFTFunction(emitters::IRModuleEmitter& moduleEmitter, size_t length);
        emitters::LLVMFunction GetFFTFunction(emitters::IRModuleEmitter& moduleEmitter, size_t length);

        // Hand-unrolled fixed-size versions
        emitters::LLVMFunction GetFFTFunction_2(emitters::IRModuleEmitter& moduleEmitter);
        emitters::LLVMFunction GetFFTFunction_4(emitters::IRModuleEmitter& moduleEmitter);

        // Performing FFT (either by calling a function or emitting inline code)
        void DoFFT(emitters::IRFunctionEmitter& function, size_t length, emitters::LLVMValue input, emitters::LLVMValue scratch);
        void DoRealFFT(emitters::IRFunctionEmitter& function, size_t length, emitters::LLVMValue input, emitters::LLVMValue scratch, emitters::LLVMValue complexInput);

        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;
    };
} // namespace nodes
} // namespace ell
