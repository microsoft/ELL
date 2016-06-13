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
    Exception<ErrorCodeType>::Exception(ErrorCodeType errorCode, const std::string& message) : _errorCode(errorCode), _message(message)
    {}
}