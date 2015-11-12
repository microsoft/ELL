// SequentialLineIterator.h

#pragma once

#include <string>
using std::string;

#include <iostream>
using std::istream;

#include <iterator>
using std::istream_iterator;

namespace dataset
{
    /// A helper class that holds a text snippet that was extracted from a longer text, and is aware of its delimiter
    ///
    template<char Delimiter>
    struct DelimitedText : public string {};

    /// Reads text from an input stream until the delimiter is encountered
    ///
    template<char Delimiter>
    istream& operator>> (istream& is, DelimitedText<Delimiter>& ln);

    /// An iterator that reads a long text line by line
    ///
    class SequentialLineIterator : public istream_iterator<DelimitedText<'\n'>>
    {
    public:
        /// Constructs a sequential line iterator
        ///
        SequentialLineIterator(istream& is);

        /// \returns True if the iterator is currently pointing to a valid iterate
        ///
        bool IsValid() const;

        /// Proceeds to the Next row
        ///
        void Next();

        /// \returns A const reference to the row
        ///
        const string& GetValue() const;
    };
}

#include "../tcc/SequentialLineIterator.tcc"
