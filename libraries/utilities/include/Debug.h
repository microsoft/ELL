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
#define DEBUG_THROW( condition, exception ) if(condition) throw exception
#define DEBUG_CHECK_ARGUMENTS( condition, message ) if(condition) throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, message)
#define DEBUG_CHECK_SIZES( condition, message ) if(condition) throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, message)
#else 
#define DEBUG_THROW( condition, exception ) 
#define DEBUG_CHECK_ARGUMENTS( condition, exception )
#define DEBUG_CHECK_SIZES( condition, message ) 
#endif

/// <summary> Emits a printf statement for debugging </summary>
/// <param name="function"> The function containing the statement. </param>
/// <param name="format"> The string format specifier. </param>
/// <param name="..."> The optional format arguments. </param>
///
/// <remarks>
/// Examples:
///     auto floatNumber = function.Load(floatNumberPtr);
///     DEBUG_EMIT_PRINTF(function, "floatNumber %f\n", floatNumber);
///
///     std::string someString = ...;
///     DEBUG_EMIT_PRINTF(function, someString + "\n");
/// </remarks>
#ifndef NDEBUG
#define DEBUG_EMIT_PRINTF( function, format, ... ) \
    function.GetModule().DeclarePrintf(); \
    function.Printf({ function.Literal(format), ##__VA_ARGS__ } );
#else
#define DEBUG_EMIT_PRINTF( function, format, ... )
#endif


