// SequentialLineIterator.cpp

#include "SequentialLineIterator.h"

// utilities
#include "files.h"
using utilities::OpenIfstream;

// stl
using std::getline;
using std::make_shared;
using std::move;

namespace dataset
{
    SequentialLineIterator::SequentialLineIterator(const string& filepath, char delim) : _delim(delim)
    {
        _upIstream = OpenIfstream(filepath);
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