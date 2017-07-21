////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Tokenizer.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Tokenizer.h"
#include "Exception.h"
#include "Files.h"

// stl
#include <cassert>
#include <cctype>
#include <iostream>
#include <sstream>

// Note: BUFFER_SIZE must be larger than the largest readable token 
#define BUFFER_SIZE 1024*1024

namespace ell
{
namespace utilities
{
    //
    // Tokenizer
    //
    Tokenizer::Tokenizer(std::istream& inputStream, const std::string tokenStartChars)
        : _in(inputStream), _tokenStartChars(tokenStartChars)
    {
        // Initially, start with an empty buffer
        _currentPosition = _textBuffer.begin();
        _tokenStart = _textBuffer.begin();
        _bufferEnd = _textBuffer.end();
    }

    std::string Tokenizer::ReadNextToken()
    {
        if (!_peekedTokens.empty())
        {
            auto temp = _peekedTokens.top();
            _peekedTokens.pop();
            return temp;
        }

        const char escapeChar = '\\';
        std::string token;

        // eat whitespace and add first char
        while (IsValid())
        {
            auto ch = GetNextCharacter();
            if (ch == EOF)
            {
                _tokenStart = _currentPosition;
                return token;
            }
            else if (!std::isspace(ch))
            {
                token.push_back((char)ch);
                bool isParsingString = _currentStringDelimiter != '\0';
                bool isStringDelimiter = _stringDelimiters.find(ch) != std::string::npos;
                if (isParsingString) // we're in the middle of parsing a string: probably because we just read in a quotation mark last time
                {
                    if (isStringDelimiter)
                    {
                        assert(_currentStringDelimiter == ch);
                        _currentStringDelimiter = '\0';
                        _tokenStart = _currentPosition;
                        return token; // return the end-delimiter
                    }
                    else
                    {
                        // we're good -- keep eating characters from the string and adding them to the token
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
                        _tokenStart = _currentPosition;
                        return token; // we did hit a token-stop char. Return it.
                    }
                }
            }
        }
        // At this point, the first char of a token will be in the token string

        // If we're in read-string mode, read until we get an unescaped string delimiter that matches the current string delimiter
        bool prevEscaped = false;
        while (IsValid())
        {
            auto ch = GetNextCharacter();
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
                        UngetCharacter();
                        break;
                    }
                }
            }
            else if (std::isspace(ch) || _tokenStartChars.find(ch) != std::string::npos) // not in read-string mode, break on token or space
            {
                UngetCharacter();
                break;
            }

            token.push_back((char)ch);
            prevEscaped = !prevEscaped && ch == escapeChar;
        }

        _tokenStart = _currentPosition;
        return token;
    }

    std::string Tokenizer::PeekNextToken()
    {
        if (!_peekedTokens.empty())
        {
            return _peekedTokens.top();
        }

        auto token = ReadNextToken();
        PutBackToken(token);
        return token;
    }

    void Tokenizer::PutBackToken(std::string token)
    {
        _peekedTokens.push(token);
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

    bool Tokenizer::TryMatchToken(std::string token)
    {
        auto nextToken = PeekNextToken();
        if (nextToken != token)
        {
            return false;
        }
        ReadNextToken();
        return true;
    }

    bool Tokenizer::TryMatchToken(std::string token, std::string& readToken)
    {
        readToken = PeekNextToken();
        if (readToken != token)
        {
            return false;
        }
        ReadNextToken();
        return true;
    }

    void Tokenizer::MatchToken(std::string token)
    {
        std::string readToken;
        if (!TryMatchToken(token, readToken))
        {
            throw InputException(InputExceptionErrors::badStringFormat, std::string{ "Failed to match token " } + token + ", got: " + readToken);
        }
    }

    void Tokenizer::MatchTokens(const std::initializer_list<std::string>& tokens)
    {
        for (const auto& token : tokens)
        {
            MatchToken(token);
        }
    }

    bool Tokenizer::IsValid()
    {
        bool inIsValid = (bool)_in;
        return (bool)_in || _textBuffer.size() > 0 || _peekedTokens.size() > 0;
    }

    int Tokenizer::GetNextCharacter()
    {
        if (_currentPosition == _bufferEnd)
        {
            ReadData();
        }
        
        if(_currentPosition == _bufferEnd)
        {
            return EOF;
        }

        auto result = *_currentPosition;
        ++_currentPosition;
        return result;
    }

    void Tokenizer::UngetCharacter()
    {
        assert(_currentPosition != _textBuffer.begin());
        --_currentPosition;
    }

    void Tokenizer::ReadData()
    {
        // Allocate textBuffer if it's empty
        auto oldLength = _bufferEnd - _tokenStart;
        auto oldOffset = _currentPosition - _tokenStart;
        if(_textBuffer.size() == 0)
        {
            _textBuffer.resize(BUFFER_SIZE, '\0');
            _bufferEnd = _textBuffer.end();
        }
        else
        {
            // move data from currentPosition to end to the beginning of the buffer
            std::copy(_tokenStart, _bufferEnd, _textBuffer.begin());
        }

        auto newPtr = _textBuffer.data() + oldLength;
        auto maxLength = _textBuffer.size() - oldLength;

        // read into buffer
        _in.read(newPtr, maxLength);
        auto amountRead = _in.gcount();
        _bufferEnd = _textBuffer.begin() + oldLength + amountRead;
        _tokenStart = _textBuffer.begin();
        _currentPosition = _tokenStart + oldOffset;
    }
}
}
