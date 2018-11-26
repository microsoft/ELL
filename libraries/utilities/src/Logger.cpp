///////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Logger.cpp (utilities)
//  Authors:  Kern Handa
//
///////////////////////////////////////////////////////////////////////////////

#include "Logger.h"

#include <fstream>
#include <iostream>

namespace ell
{
namespace utilities
{
    namespace logging
    {

        bool& ShouldFlush()
        {
            static bool shouldFlush = false;
            return shouldFlush;
        }

        bool& ShouldLog()
        {
            static bool shouldLog = false;
            return shouldLog;
        }

        std::ostream& EOL(std::ostream& stream)
        {
            stream << "\n";
            if (ShouldFlush())
            {
                stream << std::flush;
            }

            return stream;
        }

        OutputStreamImpostor& Log()
        {
            static OutputStreamImpostor nullHandle;
            static OutputStreamImpostor logHandle{ std::cout };
            if (ShouldLog())
            {
                return logHandle;
            }
            else
            {
                return nullHandle;
            }
        }

    } // namespace logging
} // namespace utilities
} // namespace ell
