////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Debug.h (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Exception.h"

#ifndef NDEBUG
#define DEBUG_THROW(condition, exception) \
    if (condition) throw exception
#define DEBUG_CHECK_ARGUMENTS(condition, message) \
    if (condition) throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, message)
#define DEBUG_CHECK_SIZES(condition, message) \
    if (condition) throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, message)
#else
#define DEBUG_THROW(condition, exception)
#define DEBUG_CHECK_ARGUMENTS(condition, exception)
#define DEBUG_CHECK_SIZES(condition, message)
#endif
