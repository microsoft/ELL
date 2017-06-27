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
        : _inputCallback(nullptr), _inputBuffer(nullptr), _outputCallback(nullptr)
    {
    }

    template <typename InputType, typename OutputType>
    void CallbackForwarder<InputType, OutputType>::InitializeOnce(CallbackBase<InputType>& inputCallback,
                                                                  std::vector<InputType>& inputBuffer,
                                                                  CallbackBase<OutputType>& outputCallback,
                                                                  size_t outputSize)
    {
        assert(_inputCallback == nullptr);
        assert(_inputBuffer == nullptr);
        assert(_outputCallback == nullptr);

        _inputCallback = &inputCallback;
        _inputBuffer = &inputBuffer;
        _outputCallback = &outputCallback;
        _outputBuffer.resize(outputSize);
    }

    template <typename InputType, typename OutputType>
    bool CallbackForwarder<InputType, OutputType>::InvokeInput(InputType* buffer)
    {
        if (_inputCallback == nullptr || _inputBuffer == nullptr)
        {
            throw std::invalid_argument("InitializeOnce has not yet been called");
        }

        bool result = _inputCallback->Run(*_inputBuffer);
        if (result)
        {
            // EFFICIENCY: any way to avoid the copy?
            std::copy(_inputBuffer->begin(), _inputBuffer->end(), buffer);
        }

        return result;
    }

    template <typename InputType, typename OutputType>
    void CallbackForwarder<InputType, OutputType>::InvokeOutput(const OutputType* buffer)
    {
        if (_outputCallback == nullptr)
        {
            throw std::invalid_argument("InitializeOnce has not yet been called");
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
            throw std::invalid_argument("InitializeOnce has not yet been called");
        }

        if (_outputBuffer.size() < 1)
        {
            throw std::out_of_range("Output buffer is not initialized correctly");
        }

        _outputBuffer[0] = value;
        _outputCallback->Run(_outputBuffer);
    }
}
}
