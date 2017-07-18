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
    class MillisecondTimer
    {
    public:
        MillisecondTimer()
            : _start(std::chrono::system_clock::now()), _running(true) {}

        void Start()
        {
            _start = std::chrono::system_clock::now();
            _running = true;
        }

        void Stop()
        {
            _end = std::chrono::system_clock::now();
            _running = false;
        }

        void Reset()
        {
            _start = std::chrono::system_clock::now();
            _end = _start;
        }

        std::chrono::milliseconds::rep Elapsed()
        {
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
