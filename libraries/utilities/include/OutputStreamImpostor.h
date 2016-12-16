////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputStreamImpostor.h (utilities)
//  Authors:  Chuck Jacobs, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <fstream>
#include <memory>
#include <ostream>
#include <string>

namespace ell
{
namespace utilities
{
    /// <summary> An imposter class that can stand in for a std::ostream object </summary>
    class OutputStreamImpostor
    {
    public:
        /// <summary> Types of standard output streams, that are not file streams. </summary>
        enum class StreamType
        {
            cout,
            cerr,
            null
        };

        OutputStreamImpostor() = default;
        OutputStreamImpostor(const OutputStreamImpostor&) = default;

        /// <summary> Constructor that creates an object that directs output to a specified stream. </summary>
        ///
        /// <param name="streamType"> Type of the stream, defaults to the null stream. </param>
        OutputStreamImpostor(StreamType streamType);

        /// <summary> Constructor that creates an object that directs output to a file</summary>
        ///
        /// <param name="filename">A filename</param>
        OutputStreamImpostor(std::string filename);

        /// <summary> Casting operator that returns a reference to an ostream. This allows us to use an OutputStreamImpostor
        /// in most places where an ostream would be accepted. </summary>
        operator std::ostream&() & { return *_outputStream; }

        /// <summary> Casting operator that returns a const reference to an ostream. This allows us to use an OutputStreamImpostor
        /// in most places where a const ostream reference would be accepted. </summary>
        operator std::ostream const&() const& { return *_outputStream; }

        /// <summary> Output operator that sends the given value to the output stream </summary>
        ///
        /// <param name="value"> The value to output </param>
        template <typename T>
        OutputStreamImpostor& operator<<(const T& value);

    private:
        std::shared_ptr<std::ofstream> _outputFileStream;
        std::shared_ptr<std::ostream> _outputStream;
    };
}
}

#include "../tcc/OutputStreamImpostor.tcc"
