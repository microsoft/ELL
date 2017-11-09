///////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Logger.h (utilities)
//  Authors:  Kern Handa
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "OutputStreamImpostor.h"

namespace ell
{
namespace utilities
{
namespace logging
{
    //// <summary> Inserts a newline character into the output sequence os and
    /// flushes it as if by calling os.put(os.widen('\n')) followed by os.flush(),
    /// if ShouldFlush() returns true.
    ///
    /// This is an output-only I/O manipulator, it may be called with an expression
    /// such as out << EOL for any out of type std::ostream. </summary>
    std::ostream& EOL(std::ostream& stream);

    /// <summary> Sets the global state for whether `EOL` results in flushing
    /// the stream.
    ///
    /// To enable flushing, set the return value of the function to true.
    ///
    /// ```cpp
    /// ShouldFlush() = true;
    /// ```
    ///
    ///  </summary>
    ///
    /// <returns> Returns a reference to the global state controlling flushing </returns>
    bool& ShouldFlush();

    /// <summary> Sets the global state for whether outputting data to the
    /// referenced returned by `Logger` produces any logs.
    ///
    /// To enable logging, set the return value of the function to true.
    ///
    /// ```cpp
    /// ShouldLog() = true;
    /// ```
    ///
    ///  </summary>
    ///
    /// <returns> Returns a reference to the global state controlling logging </returns>
    bool& ShouldLog();

    /// <summary> Returns a reference to a std::ostream which can be used
    /// to log anything that supports writing to a std::ostream object.
    ///
    /// The std::ostream that Logger references can be replaced, but can
    /// only be done when ShouldLog() is set to true.
    ///
    /// ```cpp
    /// std::ofstream f("log.txt", std::ios::binary);
    /// Log() = f;
    /// ```
    ///
    /// NB: The reference is thread-safe, the underlying object may not be.
    /// </summary>
    ///
    /// <returns> Returns a reference to a global OutputStreamImpostor object </returns>
    OutputStreamImpostor& Log();
}
}
namespace logging = utilities::logging;
}
