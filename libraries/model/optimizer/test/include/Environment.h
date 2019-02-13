////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Environment.h (model/optimizer)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <emitters/include/TargetDevice.h>

#include <optional>

namespace ell
{
namespace model
{
    namespace optimizer
    {
        /// <summary>
        /// Environment represents a runtime environment we want to optimize for.
        /// It can include information about the target machine, or about the data it will be run on.
        /// </summary>
        class Environment
        {
        public:
            Environment();

            /// <summary> Constructor with a target device. </summary>
            Environment(const emitters::TargetDevice& targetDevice);

            /// <summary> Returns `true` if this environment has a target device specified. </summary>
            bool HasTargetDevice() const;

            /// <summary> Get the target device for this environment. Throws an exception if there isn't one. </summary>
            const emitters::TargetDevice& GetTargetDevice() const;

        private:
            std::optional<emitters::TargetDevice> _targetDevice;
        };
    } // namespace optimizer
} // namespace model
} // namespace ell
