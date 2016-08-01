////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Tokenizer.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Tokenizer.h"
#include "Exception.h"

// stl
#include <cassert>
#include <sstream>
#include <iostream>

namespace utilities
{
    // 

    //
    // Tokenizer
    //
    std::string Tokenizer::ReadNextToken()
    {
        if (_peekedTokens.size() > 0)
        {
            auto temp = _peekedTokens.back();
            _peekedTokens.pop_back();
            return temp;
        }
        std::stringstream tokenStream;

        // Problem: we won't have any way to distinguish between a string with a single quote in it and a string delimiter token.
        // need a token class with a token_type field that is either "identifier" or "string"

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
                bool isStringDelimiter = _stringDelimiters.find(ch) != std::string::npos;
                if(_currentStringDelimiter == '\0') // didn't just finish parsing a string, so set the delimiter if we got one
                {
                    _currentStringDelimiter = isStringDelimiter ? ch : '\0';
                }
                else // did just finished parsing a delimiter or a string 
                {
                    if(isStringDelimiter)
                    {
                        assert(_currentStringDelimiter == ch);
                        _currentStringDelimiter = '\0';
                    }
                    else
                    {
                        // we're good.
                    }
                }

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

            if(!prevEscaped && _currentStringDelimiter != '\0')
            {         
                // only break if we're done reading a string
                if(ch == _currentStringDelimiter)
                {
                    _in.unget();
                    break;
                }
            }
            else if (std::isspace(ch) || _tokenStartChars.find(ch) != std::string::npos)
            {
                _in.unget();
                break;
            }

            if(!prevEscaped)
            {
                tokenStream << (char)ch;
            }

            prevEscaped = !prevEscaped && ch == '\\';
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

    void Tokenizer::MatchNextToken(std::string value)
    {
        auto token = ReadNextToken();
        if (token != value)
        {
            throw InputException(InputExceptionErrors::badStringFormat, std::string{"Failed to match token "} + value + ", got: " + token);
        }
    }
}
