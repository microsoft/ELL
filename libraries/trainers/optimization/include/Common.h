////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Common.h (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <exception>
#include <string>

#ifndef NDEBUG
#define DEBUG_CHECK(goodCondition, message) \
    if (!(goodCondition)) throw OptimizationException(message)
#else
#define DEBUG_CHECK(condition, exception)
#endif

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        class OptimizationException : public std::exception
        {
        public:
            /// <summary></summary>
            OptimizationException(const std::string& message) :
                _message(message) {}

            /// <summary> Returns the message. </summary>
            virtual const std::string& GetMessage() const { return _message; }

        private:
            std::string _message;
        };
    } // namespace optimization
} // namespace trainers
} // namespace ell
