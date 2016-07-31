////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Tokenizer.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <istream>
#include <string>
#include <vector>

namespace utilities
{
    class Tokenizer
    {
    public:
        Tokenizer(std::istream& in, const std::string tokenStopChars): _in(in), _tokenStopChars(tokenStopChars) {}

        std::string ReadNextToken(); // returns "" at EOF
        void PutBackToken(std::string token);
        std::string PeekNextToken(); // returns "" at EOF
        void MatchNextToken(std::string readString); // throws an exception if it doesn't match
        void PrintTokens();

    private:
        std::istream& _in;
        std::string _tokenStopChars;
        std::string _stringDelimiters = "'\"";

        std::vector<std::string> _peekedTokens;

        char _currentStringDelimiter = '\0'; // '\0' if we're not currently parsing a string
    };
}
