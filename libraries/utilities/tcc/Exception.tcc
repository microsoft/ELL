////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Exception.tcc (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    template<typename ErrorCodeType>
    ExceptionBase<ErrorCodeType>::ExceptionBase(ErrorCodeType errorCode, const std::string& message) : _errorCode(errorCode), _message(message)
    {}
}