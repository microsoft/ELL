////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Exception.h (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

namespace utilities
{
    /// <summary> Base class for exceptions. </summary>
    ///
    /// <typeparam name="ErrorCodeType"> An enum class with error codes. </typeparam>
    template <typename ErrorCodeType>
    class ExceptionBase 
    {
    public:

        /// <summary> Constructs an exception with a give error code from the enum ErrorCodeType. </summary>
        ///
        /// <param name="errorCode"> The error code from ErrorCodeType. </param>
        /// <param name="message"> A message. </param>
        ExceptionBase(ErrorCodeType errorCode, const std::string& message = "");

        /// <summary> Gets the error code. </summary>
        ///
        /// <returns> The error code. </returns>
        ErrorCodeType GetErrorCode() const noexcept { return _errorCode; };

        /// <summary> Gets the message. </summary>
        ///
        /// <returns> A message string; </returns>
        const std::string& GetMessage() const noexcept { return _message; }

    private:
        ErrorCodeType _errorCode;
        std::string _message;
    };

    /// <summary> General exception error codes. </summary>
    enum class ExceptionErrorCodes
    {
        illegalState,
        badStringFormat,
        invalidArgument,
        indexOutOfRange,
        divideByZero,
        typeMismatch,
        functionNotImplemented,
        nullReference,
        notYetImplemented
    };

    typedef ExceptionBase<ExceptionErrorCodes> Exception;
}

#include "../tcc/Exception.tcc"

