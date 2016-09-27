////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputStreamImpostor.tcc (utilities)
//  Authors:  Chuck Jacobs, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
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
