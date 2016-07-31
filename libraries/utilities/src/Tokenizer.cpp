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
#include <sstream>
#include <iostream>

namespace utilities
{
    //
    // Tokenizer
    //
    std::string Tokenizer::ReadNextToken()
    {
        // TODO: coalesce whitespace, but skip it if not in string mode
        if (_peekedTokens.size() > 0)
        {
            auto temp = _peekedTokens.back();
            _peekedTokens.pop_back();
            return temp;
        }

        const std::string whitespace = " \r\t\n";
        const std::string tokenStopChars = " \t\r\n<>=/'\"";
        std::stringstream tokenStream;

        // eat whitespace and add first char
        while (_in)
        {
            auto ch = _in.get();
            if (ch == EOF)
                return "";
            if (!std::isspace(ch))
            {
                tokenStream << (char)ch;
                if (tokenStopChars.find(ch) == std::string::npos)
                    break;
                else
                    return tokenStream.str();
            }
        }

        while (_in)
        {
            auto ch = _in.get();
            if (ch == EOF)
            {
                break;
            }

            if (tokenStopChars.find(ch) != std::string::npos)
            {
                _in.unget();
                break;
            }
            tokenStream << (char)ch;
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
