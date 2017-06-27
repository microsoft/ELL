////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CallbackInterface.h (interfaces)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef SWIG

// stl
#include <vector>

#endif

namespace ell
{
namespace api
{
    // The base callback interface enabled with the SWIG director feature.
    // Via cross-language polymorphism, callbacks written in the caller's
    // language derive from this class and are then called by CallbackForwarder.
    template <typename ElementType>
    class CallbackBase
    {
    public:
        virtual ~CallbackBase() = default;

        /// <summary> Runs the callback. Derived classes should override. </summary>
        ///
        /// <param name="buffer"> The callback data buffer. </param>
        /// <returns> The callback status. </param>
        virtual bool Run(std::vector<ElementType>& /*buffer*/)
        {
            // STYLE this can't be pure virtual in order for SWIG to generate wrappers
            // so we provide a default implementation that returns false
            return false;
        }
    };

    // The interface that forwards callback invocations from emitted code
    // over to the language-specific callback implementations (typically, derived
    // classes of CallbackBase).
    // Known limitations:
    // * Assumes: BOTH in and out callbacks are always present in the model (Ideal: allow any to be optional)
    // * Assumes: ONE instance per callback type (Ideal: support multiple instances)
    template <typename InputType, typename OutputType>
    class CallbackForwarder
    {
    public:
        /// <summary> Constructor </summary>
        CallbackForwarder();

        virtual ~CallbackForwarder() = default;

        /// <summary> Invokes the input callback </summary>
        ///
        /// <param name="buffer"> The callback data buffer. </param>
        /// <returns> The callback status. </param>
        bool InvokeInput(InputType* buffer);

        /// <summary> Invokes the output callback </summary>
        ///
        /// <param name="buffer"> The callback data buffer. </param>
        void InvokeOutput(const OutputType* buffer);

        /// <summary> Invokes the output callback with a scalar value </summary>
        ///
        /// <param name="buffer"> The callback scalar value. </param>
        void InvokeOutput(OutputType value);

    protected:
        /// <summary> Performs a one-time initialization of the forwarder </summary>
        ///
        /// <param name="inputCallback"> The input callback object. </param>
        /// <param name="inputBuffer"> The buffer used by the input callback object. Caller controls both the lifetime of the buffer and this object. </param>
        /// <param name="outputCallback"> The output callback object. </param>
        /// <param name="outputSize"> The number of output values. </param>
        ///
        /// STYLE SWIG pythonprepend requires the parameters to be fully qualified </param>
        void InitializeOnce(ell::api::CallbackBase<InputType>& inputCallback,
                            std::vector<InputType>& inputBuffer,
                            ell::api::CallbackBase<OutputType>& outputCallback,
                            size_t outputSize);

    private:
        // Raw pointers are used because lifetime management is performed by the caller
        CallbackBase<InputType>* _inputCallback;
        std::vector<InputType>* _inputBuffer;
        CallbackBase<OutputType>* _outputCallback;

        std::vector<OutputType> _outputBuffer;
    };
}
}

#include "../tcc/CallbackInterface.tcc"
