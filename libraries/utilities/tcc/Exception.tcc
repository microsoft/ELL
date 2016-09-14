////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LogicException.tcc (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
namespace utilities
{
    template <typename ErrorCodeType>
    ErrorCodeException<ErrorCodeType>::ErrorCodeException(ErrorCodeType errorCode, const std::string& message)
        : _errorCode(errorCode), _message(message)
    {
    }
}
}
