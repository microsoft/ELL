////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Tokenizer.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <initializer_list>
#include <istream>
#include <stack>
#include <string>
#include <vector>

namespace ell
{
namespace utilities
{
    /// <summary> A very simple tokenizer suitable for XML and JSON deserialization </summary>
    class Tokenizer
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name=inputStream> Stream to read from </param>
        /// <param name=tokenStartChars> Set of characters that indicate the beginning of a new token. </param>
        Tokenizer(std::istream& inputStream, const std::string tokenStartChars);

        /// <summary> Gets the next token from the input stream. </summary>
        ///
        /// <returns> The next token, or the empty string if the end of file is reached. </returns>
        std::string ReadNextToken();

        /// <summary> Returns a token back to the input stream. </summary>
        ///
        /// <param name="token"> The token to return to the stream. </param>
        void PutBackToken(std::string token);

        /// <summary> Matches the next token from the input stream. Throws an exception if token doesn't match. </summary>
        ///
        /// <param name="token"> The token to match. </param>
        bool TryMatchToken(std::string token);
        
        /// <summary> Matches the next token from the input stream. Throws an exception if token doesn't match. </summary>
        ///
        /// <param name="token"> The token to match. </param>
        /// <param name="readToek"> The token actually read. </param>
        bool TryMatchToken(std::string token, std::string& readToken);
        
        /// <summary> Matches the next token from the input stream. Throws an exception if token doesn't match. </summary>
        ///
        /// <param name="token"> The token to match. </param>
        void MatchToken(std::string token);

        /// <summary> Matches the next token from the input stream. Throws an exception if token doesn't match. </summary>
        ///
        /// <param name="token"> The token to match. </param>
        void MatchTokens(const std::initializer_list<std::string>& tokens);

        /// <summary> Gets the next token from the input stream without consuming it. </summary>
        ///
        /// <returns> The next token, or the empty string if the end of file is reached. </returns>
        std::string PeekNextToken();

        /// <summary> Consumes entire stream, printing tokens as they're read. For debugging. </summary>
        ///
        /// <param name="os"> The stream to print the tokens to. </param>
        void PrintTokens(std::ostream& os);

    private:
        std::istream& _in;
        std::string _tokenStartChars;
        std::string _stringDelimiters = "'\"";

        bool IsValid();
        int GetNextCharacter();
        void UngetCharacter();
        void ReadData();

        std::vector<char> _textBuffer;
        std::vector<char>::iterator _tokenStart;
        std::vector<char>::iterator _currentPosition;
        std::vector<char>::iterator _bufferEnd;

        std::stack<std::string> _peekedTokens;

        char _currentStringDelimiter = '\0'; // '\0' if we're not currently parsing a string
    };
}
}
