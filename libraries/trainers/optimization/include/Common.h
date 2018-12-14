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
#include <utility>

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
        /// <summary> Exception class for the optimization library </summary>
        class OptimizationException : public std::exception
        {
        public:
            /// <summary> Constructor </summary>
            OptimizationException(const std::string& message) :
                _message(message) {}

            /// <summary> Returns the message. </summary>
            virtual const std::string& GetMessage() const { return _message; }

            /// <summary> Returns the message. </summary>
            const char* what() const noexcept(noexcept(std::declval<std::exception>().what())) override
            {
                return _message.c_str();
            }

        private:
            std::string _message;
        };

        /// <summary> enum class used to determine if the objective is to maximize or minimize </summary>
        enum class Objective { minimize, maximize };

    } // namespace optimization
} // namespace trainers
} // namespace ell
