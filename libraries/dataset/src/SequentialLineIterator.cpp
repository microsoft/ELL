// SequentialLineIterator.cpp

#include "SequentialLineIterator.h"

using std::getline;
using std::make_shared;
using std::move;

namespace dataset
{
    SequentialLineIterator::SequentialLineIterator(unique_ptr<istream>&& upIstream, char delim) : _upIstream(move(upIstream)), _delim(delim)
    {
        Next();
    }

    bool SequentialLineIterator::IsValid() const
    {
        return (_spCurrentLine != nullptr);
    }

    void SequentialLineIterator::Next()
    {
        auto spNextLine = make_shared<string>();
        getline(*_upIstream, *spNextLine, _delim);
        if(_upIstream->fail())
        {
            _spCurrentLine = nullptr;
        }
        else
        {
            _spCurrentLine = spNextLine;
        }
    }

    shared_ptr<const string> SequentialLineIterator::Get() const
    {
        return _spCurrentLine;
    }
}