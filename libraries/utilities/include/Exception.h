////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Exception.h (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdexcept>
#include <string>

namespace utilities
{
    template <typename ErrorCodeType>
    class Exception : public std::exception
    {
    public:

        Exception(ErrorCodeType errorCode, const std::string& message = "");

        ErrorCodeType GetErrorCode() const noexcept { return _errorCode; };

        const std::string& What() const noexcept { return _message; }

    private:
        ErrorCodeType _errorCode;
        std::string _message;
    };
}

#include "../tcc/Exception.tcc"

