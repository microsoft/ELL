////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputStreamImpostor.h (utilities)
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

        /// <summary> Casting operator that returns a reference to an ostream. This allows us to use an OutputStreamImpostor 
        /// in most places where an ostream would be accepted. </summary>
        operator std::ostream&() &;

        /// <summary> Casting operator that returns a const reference to an ostream. This allows us to use an OutputStreamImpostor 
        /// in most places where a const ostream reference would be accepted. </summary>
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
    OutputStreamImpostor GetOutputStreamImpostor(std::string filenameOrEmpty);
}
