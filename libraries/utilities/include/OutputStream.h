////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     OutputStream.h (common)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <string>
#include <ostream>
#include <fstream>
#include <memory>

namespace utilities
{
    /// <summary> An imposter class that can stand in for a std::ostream object </summary>
    class OutputStreamImpostor
    {
    public:
        /// <summary> Constructor that creates an object that directs output to std::cout </summary>
        OutputStreamImpostor();

        /// <summary> Constructor that creates an object that directs output to a file (if filenameOrEmpty points to a file), or std::cout (if filenameOrEmpty is empty)</summary>
        ///
        /// <param name="filenameOrEmpty">Either a filename or the empty string</param>
        OutputStreamImpostor(std::string filenamOrEmpty);

        operator std::ostream&() &;

        operator std::ostream const&() const &;

    private:
        std::ofstream _outputFileStream;
        std::streambuf *_outBuf;
        std::unique_ptr<std::ostream> _out;
    };

    /// <summary> Returns an object that can stand in for a std::ostream. It directs output either to a file stream or to std::cout. </summary>
    ///
    /// <param name="filenameOrEmpth"> The filename or an empty string. </param>
    ///
    /// <returns> The imposter for the stream. </returns>
    OutputStreamImpostor GetOutputStream(std::string filenameOrEmpty);
}
