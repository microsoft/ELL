////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CallbackInterface.h (interfaces)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef SWIG

#include <vector>

#endif

namespace ell
{
namespace api
{
    using TimeTickType = double;

    /// <summary>
    /// The base callback interface enabled with the SWIG director feature.
    /// Via cross-language polymorphism, callbacks written in the caller's
    /// language derive from this class.
    /// </summary>
    template <typename ElementType>
    class CallbackBase
    {
    public:
        // Defining the ctor tells SWIG to not generate duplicate wrappers
        // (one per template specialization)
        CallbackBase() = default;
        virtual ~CallbackBase() = default;

        /// <summary> This is called when callback has a vector value. Derived classes should override. </summary>
        ///
        /// <param name="buffer"> The callback data buffer. </param>
        /// <returns> The callback status. Return True to stop the model. </param>
        virtual bool Run(std::vector<ElementType>& /*buffer*/)
        {
            return false;
        }

        /// <summary> This is called when callback has a scalar value. Derived classes should override. </summary>
        ///
        /// <param name="value"> The callback data value. </param>
        /// <returns> The callback status. Return True to stop the model. </param>
        virtual bool Run(ElementType /*value*/) const
        {
            return false;
        }
    };

} // namespace api
} // namespace ell
