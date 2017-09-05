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
    std::ostream& OutputStreamImpostor::operator<<(T&& value)
    {
        *_outputStream << value;
        return *_outputStream;
    }
}
}
