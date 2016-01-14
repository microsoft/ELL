// SequentialLineIterator.h

#pragma once

#include <string>
using std::string;

#include <iostream>
using std::istream;

#include <memory>
using std::shared_ptr;
using std::unique_ptr;

namespace dataset
{
    /// An iterator that reads a long text line by line
    ///
    class SequentialLineIterator 
    {
    public:
        /// Constructs a sequential line iterator
        ///
        SequentialLineIterator(const string& filepath, char delim = '\n');

        /// deleted copy Ctor
        ///
        SequentialLineIterator(const SequentialLineIterator&) = delete; // this ctor is deleted because a provate member of this class cannot be copied

        /// default move Ctor
        ///
        SequentialLineIterator(SequentialLineIterator&&) = default;

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
        unique_ptr<istream> _upIstream;
        char _delim;
    };
}
