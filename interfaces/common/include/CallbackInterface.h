////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CallbackInterface.h (interfaces)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef SWIG

// stl
#include <vector>

#endif

namespace ell
{
namespace api
{
    namespace common
    {
        // The base callback interface enabled with the SWIG director feature.
        // Via cross-language polymorphism, callbacks written in the caller's
        // language derive from this class and are then called by CallbackForwarder.
        template <typename ElementType>
        class CallbackBase
        {
        public:
            /// <summary> Destructor </summary>
            virtual ~CallbackBase() = default;

            /// <summary> Runs the callback. Derived classes should override. </summary>
            ///
            /// <param name="buffer"> The callback data buffer. </param>
            /// <returns> The callback status. </param>
            virtual bool Run(std::vector<ElementType>& /*buffer*/)
            {
                // STYLE this can't be pure virtual in order for SWIG to generate wrappers
                // so we provide a default implementation that returns false
                return false;
            }
        };

        // The interface that forwards callback invocations from emitted code
        // over to the language-specific callback implementations (typically, derived
        // classes of CallbackBase).
        template <typename ElementType>
        class CallbackForwarder
        {
        public:
            /// <summary> Constructor </summary>
            CallbackForwarder();

            /// <summary> Destructor </summary>
            virtual ~CallbackForwarder() = default;

            /// <summary> Invokes the callback </summary>
            ///
            /// <param name="buffer"> The callback data buffer. </param>
            /// <returns> The callback status. </param>
            bool Invoke(ElementType* buffer);

        protected:
            /// <summary> Performs a one-time initialization of the forwarder </summary>
            ///
            /// <param name="callback"> The callback object. </param>
            /// <param name="buffer"> The buffer used by the callback object. </param>
            /// STYLE SWIG pythonprepend requires the parameters to be fully qualified </param>
            void InitializeOnce(ell::api::common::CallbackBase<ElementType>& callback, std::vector<ElementType>& buffer);

        private:
            std::vector<ElementType>* _buffer;
            CallbackBase<ElementType>* _callback;
        };
    }
}
}

#include "../tcc/CallbackInterface.tcc"