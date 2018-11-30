////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MillisecondTimer.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>

namespace ell
{
namespace utilities
{
    /// <summary></summary>
    class MillisecondTimer
    {
    public:
        /// <summary> Create a timer object. The timer is running when created. </summary>
        MillisecondTimer();

        /// <summary> Start the timer, resetting elapsed time to zero. </summary>
        void Start();

        /// <summary> Stop the timer, but remember the elapsed time. </summary>
        void Stop();

        /// <summary> Start the timer, accumulating any elapsed time. </summary>
        void Restart();

        /// <summary> Reset the timer to zero. </summary>
        void Reset();

        /// <summary> Return the amount of elapsed time. </summary>
        std::chrono::milliseconds::rep Elapsed();

    private:
        std::chrono::system_clock::time_point Now();
        std::chrono::system_clock::duration TimeSinceStart();

        std::chrono::system_clock::time_point _start;
        std::chrono::system_clock::duration _elapsedTime;
        bool _running;
    };
} // namespace utilities
} // namespace ell
