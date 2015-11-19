// SequentialLineIterator.h

#pragma once

#include <string>
using std::string;

#include <iostream>
using std::istream;

#include <memory>
using std::shared_ptr;

namespace dataset
{
    /// An iterator that reads a long text line by line
    ///
    class SequentialLineIterator 
    {
    public:
        /// Constructs a sequential line iterator
        ///
        SequentialLineIterator(istream& istream, char delim = '\n');

        /// \returns True if the iterator is currently pointing to a valid iterate
        ///
        bool IsValid() const;

        /// Proceeds to the Next row
        ///
        void Next();

        /// \returns A const reference to the row
        ///
        shared_ptr<const string> Get() const;

    private:
        shared_ptr<string> _spCurrentLine = nullptr;
        istream& _istream;
        char _delim;
    };
}
