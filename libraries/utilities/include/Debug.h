////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Debug.h (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef NDEBUG
#define DEBUG_THROW( condition, exception ) if(condition) throw exception
#else 
#define DEBUG_THROW( condition, exception ) 
#endif

/// <summary> Emits a printf statement for debugging </summary>
/// <param name="function"> The function containing the statement. </param>
/// <param name="format"> The string format specifier. </param>
/// <param name="..."> The optional format arguments. </param>
#ifndef NDEBUG
#define DEBUG_EMIT_PRINTF( function, format, ... ) \
    function.GetModule().DeclarePrintf(); \
    function.Printf({ function.Literal(format), ##__VA_ARGS__ } );
#else
#define DEBUG_EMIT_PRINTF( function, format, ... )
#endif


