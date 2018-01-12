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
    using TimeTickType = double;

    /// <summary>
    /// The base callback interface enabled with the SWIG director feature.
    /// Via cross-language polymorphism, callbacks written in the caller's
    /// language derive from this class and are then called by CallbackForwarder.
    /// </summary>
    template <typename ElementType>
    class CallbackBase
    {
    public:
        // Defining the ctor tells SWIG to not generate duplicate wrappers
        // (one per template specialization)
        CallbackBase() = default;
        virtual ~CallbackBase() = default;

        /// <summary> Runs the callback. Derived classes should override. </summary>
        ///
        /// <param name="buffer"> The callback data buffer. </param>
        /// <returns> The callback status. </param>
        virtual bool Run(std::vector<ElementType>& /*buffer*/)
        {
            return false;
        }

        /// <summary> Runs the callback. Derived classes should override. </summary>
        ///
        /// <param name="value"> The callback data value. </param>
        /// <returns> The callback status. </param>
        virtual bool Run(ElementType /*value*/) const
        {
            return false;
        }
    };

    /// <summary>
    /// The interface that forwards callback invocations from emitted code
    /// over to the language-specific callback implementations (typically, derived
    /// classes of CallbackBase).
    /// Known limitations:
    /// * Assumes: BOTH in and out callbacks are always present in the model (Ideal: allow any to be optional)
    /// * Assumes: ONE instance per callback type (Ideal: support multiple instances)
    /// </summary>
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

        /// <summary> Invokes the lag notification callback </summary>
        ///
        /// <param name="value"> The time lag. </param>
        void InvokeLagNotification(TimeTickType value);

        /// <summary> Registers callbacks with the forwarder </summary>
        ///
        /// <param name="inputCallback"> The input callback object. </param>
        /// <param name="inputSize"> The input size. </param>
        /// <param name="outputCallback"> The output callback object. </param>
        /// <param name="outputSize"> The output size. </param>
        /// <param name="lagCallback"> The lag callback object. </param>
        void Register(ell::api::CallbackBase<InputType>& inputCallback,
            size_t inputSize,
            ell::api::CallbackBase<OutputType>& outputCallback,
            size_t outputSize,
            ell::api::CallbackBase<TimeTickType>& lagCallback);

        /// <summary> Clears callbacks with the forwarder. </summary>
        void Clear();

    private:
        // Raw pointers are used because lifetime management is performed by the caller
        CallbackBase<InputType>* _inputCallback;
        CallbackBase<OutputType>* _outputCallback;
        CallbackBase<TimeTickType>* _lagCallback;

        std::vector<InputType> _inputBuffer;
        std::vector<OutputType> _outputBuffer;
    };
}
}

#include "../tcc/CallbackInterface.tcc"
