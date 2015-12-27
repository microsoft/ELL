// MappedParser.tcc

// utilities
#include "Parser.h"
using utilities::Parser;

#include <memory>
using std::move;

#include <stdexcept>
using std::runtime_error;

namespace dataset
{
    template<typename InternalParserType>
    bool MappedParser<InternalParserType>::Iterator::IsValid() const
    {
        return false;
    }

    template<typename InternalParserType>
    void MappedParser<InternalParserType>::Iterator::Next()
    {
    }

    template<typename InternalParserType>
    indexValue MappedParser<InternalParserType>::Iterator::Get() const
    {
        return indexValue{ 0 ,0 };
    }

    template<typename InternalParserType>
    MappedParser<InternalParserType>::Iterator::Iterator()
    {
        Next();
    }

    template<typename InternalParserType>
    MappedParser<InternalParserType>::Iterator MappedParser<InternalParserType>::GetIterator(shared_ptr<const string> spExampleString, const char* pStr) const
    {
        return Iterator();
    }
}