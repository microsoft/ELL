////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VoiceActivityDetectorNode.h (nodes)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/CompilableCodeNode.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/InputPort.h>
#include <model/include/ModelTransformer.h>
#include <model/include/OutputPort.h>

#include <emittable_functions/include/VoiceActivityDetector.h>

#include <value/include/FunctionDeclaration.h>

#include <memory>

namespace ell
{
namespace nodes
{
    /// <summary>
    /// A voice activity detection node that takes an mfcc vector input and produces an activity detected output signal.
    /// The output signal is an integer value where 0 means no activity and 1 means activity detected.
    /// </summary>
    class VoiceActivityDetectorNode : public model::CompilableCodeNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPortBase& input = _input;
        const model::OutputPortBase& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        VoiceActivityDetectorNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The signal to perform activity detection on </param>
        /// <param name="sampleRate"> The sample rate of incoming audio signal in kilohertz</param>
        /// <param name="frameDuration"> The frames duration (inverse of frames per second), this is not necessarily the same as windowSize / sampleRate, it also depends on the 'shift'. </param>
        /// <param name="tauUp"> The noise floor is computed by tracking the frame power.  It goes up slow, with this time constant value. </param>
        /// <param name="tauDown"> If the frame power is lower than the noise floor, it goes down fast, with this time constant value. </param>
        /// <param name="largeInput"> The exception is the case when the proportion frame power/noise floor is larger than this value. </param>
        /// <param name="gainAtt"> Then we switch to much slower adaptation by applying this value. </param>
        /// <param name="thresholdUp"> Then we compare the energy of the current frame to the noise floor. If it is thresholdUp times higher � we switch to state VOICE. </param>
        /// <param name="thresholdDown"> Then we compare the energy of the current frame to the noise floor. If it is thresholdDown times lower � we switch to state NO VOICE.  </param>
        /// <param name="levelThreshold"> Special case is when the energy of the frame is lower than levelThreshold, when we force the state to NO VOICE. </param>
        /// <summary>
        VoiceActivityDetectorNode(
            const model::OutputPortBase& input,
            double sampleRate,
            double frameDuration,
            double tauUp,
            double tauDown,
            double largeInput,
            double gainAtt,
            double thresholdUp,
            double thresholdDown,
            double levelThreshold);

        /// <summary> Gets the name of this type. </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "VoiceActivityDetectorCodeNode"; }

    protected:
        void Define(ell::value::FunctionDeclaration& fn) override;
        void DefineReset(ell::value::FunctionDeclaration& fn) override;

        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; }

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Inputs
        model::InputPortBase _input;

        // Output
        model::OutputPortBase _output;

        // the implementation of the algorithm
        mutable emittable_functions::VoiceActivityDetector _vad;
    };

} // namespace nodes
} // namespace ell
