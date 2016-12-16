////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputStreamImpostor.tcc (utilities)
//  Authors:  Chuck Jacobs, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace utilities
{
    template <typename T>
    OutputStreamImpostor& OutputStreamImpostor::operator<<(const T& value)
    {
        *_outputStream << value;
        return *this;
    }
}
}
