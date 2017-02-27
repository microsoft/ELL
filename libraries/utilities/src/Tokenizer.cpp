////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Tokenizer.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Tokenizer.h"
#include "Exception.h"

// stl
#include <cassert>
#include <cctype>
#include <iostream>
#include <sstream>

namespace ell
{
namespace utilities
{
    //
    // Tokenizer
    //
    std::string Tokenizer::ReadNextToken()
    {
        const char escapeChar = '\\';

        if (_peekedTokens.size() > 0)
        {
            auto temp = _peekedTokens.back();
            _peekedTokens.pop_back();
            return temp;
        }
        std::stringstream tokenStream;

        // eat whitespace and add first char
        while (_in)
        {
            auto ch = _in.get();
            if (ch == EOF)
            {
                return tokenStream.str();
            }
            else if (!std::isspace(ch))
            {
                tokenStream << (char)ch;
                bool isParsingString = _currentStringDelimiter != '\0';
                bool isStringDelimiter = _stringDelimiters.find(ch) != std::string::npos;
                if (isParsingString) // we're in the middle of parsing a string: probably because we just read in a quotation mark last time
                {
                    if (isStringDelimiter)
                    {
                        assert(_currentStringDelimiter == ch);
                        _currentStringDelimiter = '\0';
                        return tokenStream.str(); // return the end-delimiter
                    }
                    else
                    {
                        // we're good, keep eating characters from the string and adding them to the token
                        break;
                    }
                }
                else // not parsing a string currently, set the string delimiter if we hit one
                {
                    _currentStringDelimiter = isStringDelimiter ? ch : '\0';

                    if (_tokenStartChars.find(ch) == std::string::npos) // if we didn't hit a token-stop char, break out of this loop and keep reading
                    {
                        break;
                    }
                    else
                    {
                        return tokenStream.str(); // we did hit a token-stop char. Return it.
                    }
                }
            }
        }
        // At this point, the first char of a token will be in the tokenStream

        // If we're in read-string mode, read until we get an unescaped string delimiter that matches the current string delimiter
        bool prevEscaped = false;
        while (_in)
        {
            auto ch = _in.get();
            if (ch == EOF)
            {
                break;
            }

            if (_currentStringDelimiter != '\0') // we're in read-string mode
            {
                if (!prevEscaped)
                {
                    // only break if we're done reading a string
                    if (ch == _currentStringDelimiter)
                    {
                        _in.unget();
                        break;
                    }
                }
            }
            else if (std::isspace(ch) || _tokenStartChars.find(ch) != std::string::npos) // not in read-string mode, break on token or space
            {
                _in.unget();
                break;
            }

            tokenStream << (char)ch;
            prevEscaped = !prevEscaped && ch == escapeChar;
        }

        return tokenStream.str();
    }

    std::string Tokenizer::PeekNextToken()
    {
        auto token = ReadNextToken();
        PutBackToken(token);
        return token;
    }

    void Tokenizer::PutBackToken(std::string token)
    {
        _peekedTokens.push_back(token);
    }

    void Tokenizer::PrintTokens()
    {
        while (true)
        {
            auto token = ReadNextToken();
            if (token == "")
                break;
            std::cout << "Token: " << token << std::endl;
        }
    }

    void Tokenizer::MatchToken(std::string token)
    {
        auto nextToken = ReadNextToken();
        if (nextToken != token)
        {
            std::cout << "Failed to match token " << token << ", got: " << nextToken << std::endl;
            assert(false);
            throw InputException(InputExceptionErrors::badStringFormat, std::string{ "Failed to match token " } + token + ", got: " + nextToken);
        }
    }

    void Tokenizer::MatchTokens(const std::initializer_list<std::string>& tokens)
    {
        for (const auto& token : tokens)
        {
            MatchToken(token);
        }
    }
}
}
