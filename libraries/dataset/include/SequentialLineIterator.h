////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     SequentialLineIterator.h (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <string>
#include <fstream>
#include <memory>

namespace dataset
{
    /// An iterator that reads a long text line by line
    ///
    class SequentialLineIterator 
    {
    public:
        /// Constructs a sequential line iterator
        ///
        SequentialLineIterator(const std::string& filepath, char delim = '\n');

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
        std::shared_ptr<const std::string> Get() const;

    private:
        std::shared_ptr<std::string> _spCurrentLine = nullptr;
        std::ifstream _iFStream;
        char _delim;
    };
}
