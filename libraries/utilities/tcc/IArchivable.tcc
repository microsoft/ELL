////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IArchivable.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    template <typename ValueType>
    static ValueType CreateObject(const Archiver& archiver)
    {
        // TODO: force archiver to "deserialize" if necessary (?)
        return archiver.GetValue<ValueType>();
    }
}
