////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VoiceActivityDetectorNode.h (nodes)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNode.h"
#include "IRMapCompiler.h"
#include "InputPort.h"
#include "ModelTransformer.h"
#include "OutputPort.h"

// dsp
#include "VoiceActivityDetector.h"

// stl
#include <memory>

namespace ell
{
namespace nodes
{
    /// <summary> 
    /// A voice activity detection node that takes an mfcc vector input and produces an activity detected output signal.
    /// The output signal is an integer value where 0 means no activity and 1 means activity detected.
    /// </summary>
    template <typename ValueType>
    class VoiceActivityDetectorNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<int>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        VoiceActivityDetectorNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The signal to perform activity detection on </param>
        /// <param name="sampleRate"> The sample rate of incoming audio signal in kilohertz</param>
        /// <param name="frameDuration"> The frames duration (inverse of frames per second), this is not necessarily the same as windowSize / sampleRate, it also depends on the 'shift'. </param>
        /// <param name="tauUp"> The noise floor is computed by tracking the frame power.  It goes up slow, with this time constant value. </param>
        /// <param name="tauDown"> If the frame power is lower than the noise floor – it goes down fast, with this time constant value. </param>
        /// <param name="largeInput"> The exception is the case when the proportion frame power/noise floor is larger than this value. </param>
        /// <param name="gainAtt"> Then we switch to much slower adaptation by applying this value. </param>
        /// <param name="thresholdUp"> Then we compare the energy of the current frame to the noise floor. If it is thresholdUp times higher – we switch to state VOICE. </param>
        /// <param name="thresholdDown"> Then we compare the energy of the current frame to the noise floor. If it is thresholdDown times lower – we switch to state NO VOICE.  </param>
        /// <param name="levelThreshold"> Special case is when the energy of the frame is lower than levelThreshold, when we force the state to NO VOICE. </param>
        /// <summary> 
        VoiceActivityDetectorNode(
            const model::PortElements<ValueType>& input, 
            double sampleRate, 
            double frameDuration,
            double tauUp,
            double tauDown,
            double largeInput,
            double gainAtt,
            double thresholdUp,
            double thresholdDown,
            double levelThreshold);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("VoiceActivityDetectorNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        void Copy(model::ModelTransformer& transformer) const override;

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; }
    
    private:

        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<int> _output;

        // for compute implementation.
        mutable dsp::VoiceActivityDetector _vad;
    };
}
}

#include "../tcc/VoiceActivityDetectorNode.tcc"
