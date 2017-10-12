////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MillisecondTimer.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <chrono>

namespace ell
{
namespace utilities
{
    /// <summary></summary>
    class MillisecondTimer
    {
    public:
        /// <summary></summary>
        MillisecondTimer()
            : _start(std::chrono::system_clock::now()), _running(true) {}

        /// <summary></summary>
        void Start()
        {
            // STYLE discrepancy
            _start = std::chrono::system_clock::now();
            _running = true;
        }

        /// <summary></summary>
        void Stop()
        {
            // STYLE discrepancy
            _end = std::chrono::system_clock::now();
            _running = false;
        }

        /// <summary></summary>
        void Reset()
        {
            // STYLE discrepancy
            _start = std::chrono::system_clock::now();
            _end = _start;
        }

        /// <summary></summary>
        std::chrono::milliseconds::rep Elapsed()
        {
            // STYLE discrepancy
            if (_running)
            {
                _end = std::chrono::system_clock::now();
            }

            return std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start).count();
        }

    private:
        std::chrono::system_clock::time_point _start;
        std::chrono::system_clock::time_point _end;
        bool _running;
    };
}
}
