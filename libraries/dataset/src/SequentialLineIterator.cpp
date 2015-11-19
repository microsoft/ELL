// SequentialLineIterator.cpp

#include "SequentialLineIterator.h"

using std::getline;
using std::make_shared;
using std::move;

namespace dataset
{
    SequentialLineIterator::SequentialLineIterator(istream& istream, char delim) : _istream(istream), _delim(delim)
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
        getline(_istream, *spNextLine, _delim); // TODO (ofer) - make this more efficient by using char[] rather than string
        if(_istream.fail())
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