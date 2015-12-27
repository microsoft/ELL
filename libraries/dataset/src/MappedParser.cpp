// MappedParser.cpp

#include "MappedParser.h"

// utilities
#include "Parser.h"
using utilities::Parser;

#include <memory>
using std::move;

#include <stdexcept>
using std::runtime_error;

namespace dataset
{
    bool MappedParser::Iterator::IsValid() const
    {
        return false;
    }

    void MappedParser::Iterator::Next()
    {
    }

    indexValue MappedParser::Iterator::Get() const
    {
        return indexValue{ 0 ,0 };
    }

    MappedParser::Iterator::Iterator() 
    {
        Next();
    }

    MappedParser::Iterator MappedParser::GetIterator(shared_ptr<const string> spExampleString, const char* pStr) const
    {
        return Iterator();
    }
}