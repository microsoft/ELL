////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Exception.h (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <exception>
#include <string>

namespace ell
{
namespace utilities
{
    /// <summary> Base class for exceptions. </summary>
    class Exception : public std::exception
    {
    public:
        /// <summary></summary>
        Exception(const std::string& message) : _message(message) { }  // STYLE discrepancy 

        ~Exception() override = default;

        /// <summary> Gets the message. </summary>
        ///
        /// <returns> A message string; </returns>
        virtual const std::string& GetMessage() const { return _message; }
    private:
        std::string _message;
    };

    /// <summary> Base class for exceptions with error codes. </summary>
    ///
    /// <typeparam name="ErrorCodeType"> An enum class with error codes. </typeparam>
    template <typename ErrorCodeType>
    class ErrorCodeException : public Exception
    {
    public:
        /// <summary> Constructs an exception with a give error code from the enum ErrorCodeType. </summary>
        ///
        /// <param name="errorCode"> The error code from ErrorCodeType. </param>
        /// <param name="message"> A message. </param>
        ErrorCodeException(ErrorCodeType errorCode, const std::string& message = "");

        /// <summary> Gets the error code. </summary>
        ///
        /// <returns> The error code. </returns>
        ErrorCodeType GetErrorCode() const noexcept { return _errorCode; };

    private:
        ErrorCodeType _errorCode;
    };

    /// <summary> Error codes for exceptions that are the programmer's fault, namely, things that are known at compile time. </summary>
    enum class LogicExceptionErrors
    {
        illegalState,
        notImplemented,
        notInitialized
    };

    /// <summary> Error codes for exceptions that are the system's fault (missing files, serial ports, TCP ports, etc). </summary>
    enum class SystemExceptionErrors
    {
        fileNotFound,
        fileNotWritable,
        serialPortUnavailable
    };

    /// <summary> Error codes for exceptions due to the numeric values in the data. </summary>
    enum class NumericExceptionErrors
    {
        divideByZero,
        overflow,
        didNotConverge
    };

    /// <summary> Error codes for exceptions related to inputs, such as public API calls. </summary>
    enum class InputExceptionErrors
    {
        badStringFormat,
        badData,
        indexOutOfRange,
        invalidArgument,
        invalidSize,
        nullReference,
        sizeMismatch,
        typeMismatch,
        versionMismatch
    };

    enum class DataFormatErrors
    {
        badFormat,
        illegalValue,
        abruptEnd,
    };

    using LogicException = ErrorCodeException<LogicExceptionErrors>;
    using SystemException = ErrorCodeException<SystemExceptionErrors>;
    using NumericException = ErrorCodeException<NumericExceptionErrors>;
    using InputException = ErrorCodeException<InputExceptionErrors>;
    using DataFormatException = ErrorCodeException<DataFormatErrors>;
}
}

#include "../tcc/Exception.tcc"
