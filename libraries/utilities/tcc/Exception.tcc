////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LogicException.tcc (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace utilities
{
    template <typename ErrorCodeType>
    ErrorCodeException<ErrorCodeType>::ErrorCodeException(ErrorCodeType errorCode, const std::string& message) :
        Exception(message),
        _errorCode(errorCode)
    {
    }
} // namespace utilities
} // namespace ell
