////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputStreamImpostor.tcc (utilities)
//  Authors:  Chuck Jacobs, Ofer Dekel, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace utilities
{
    template <typename T>
    std::ostream& OutputStreamImpostor::operator<<(T&& value)
    {
        _outputStream.get() << value;
        return _outputStream;
    }
}
}
