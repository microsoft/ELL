////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CallbackInterface.tcc (interfaces)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SWIG

// stl
#include <assert.h>
#include <stdexcept>

#endif

namespace ell
{
namespace api
{
    //////////////////////////////////////////////////////////////////////////
    // Api classes for callback forwarding
    //////////////////////////////////////////////////////////////////////////

    template <typename InputType, typename OutputType>
    CallbackForwarder<InputType, OutputType>::CallbackForwarder()
        : _inputCallback(nullptr), _outputCallback(nullptr), _lagCallback(nullptr)
    {
    }

    template <typename InputType, typename OutputType>
    void CallbackForwarder<InputType, OutputType>::Register(CallbackBase<InputType>& inputCallback,
        size_t inputSize,
        CallbackBase<OutputType>& outputCallback,
        size_t outputSize,
        CallbackBase<TimeTickType>& lagCallback)
    {
        // Caller owns the lifetime of these objects
        _inputCallback = &inputCallback;
        _outputCallback = &outputCallback;
        _lagCallback = &lagCallback;

        _inputBuffer.resize(inputSize);
        _outputBuffer.resize(outputSize);
    }

    template <typename InputType, typename OutputType>
    void CallbackForwarder<InputType, OutputType>::Clear()
    {
        _inputCallback = nullptr;
        _outputCallback = nullptr;
        _lagCallback = nullptr;

        _inputBuffer.resize(0);
        _outputBuffer.resize(0);
    }

    template <typename InputType, typename OutputType>
    bool CallbackForwarder<InputType, OutputType>::InvokeInput(InputType* buffer)
    {
        if (_inputCallback == nullptr)
        {
            throw std::invalid_argument("Register has not yet been called");
        }

        bool result = _inputCallback->Run(_inputBuffer);
        if (result)
        {
            // EFFICIENCY: any way to avoid the copy?
            std::copy(_inputBuffer.begin(), _inputBuffer.end(), buffer);
        }
        return result;
    }

    template <typename InputType, typename OutputType>
    void CallbackForwarder<InputType, OutputType>::InvokeOutput(const OutputType* buffer)
    {
        if (_outputCallback == nullptr)
        {
            throw std::invalid_argument("Register has not yet been called");
        }

        // EFFICIENCY: any way to avoid the copy?
        _outputBuffer.assign(buffer, buffer + _outputBuffer.size());
        _outputCallback->Run(_outputBuffer);
    }

    template <typename InputType, typename OutputType>
    void CallbackForwarder<InputType, OutputType>::InvokeOutput(OutputType value)
    {
        if (_outputCallback == nullptr)
        {
            throw std::invalid_argument("Register has not yet been called");
        }

        _outputCallback->Run(value);
    }

    template <typename InputType, typename OutputType>
    void CallbackForwarder<InputType, OutputType>::InvokeLagNotification(TimeTickType value)
    {
        if (_lagCallback == nullptr)
        {
            throw std::invalid_argument("Register has not yet been called");
        }

        _lagCallback->Run(value);
    }
}
}
