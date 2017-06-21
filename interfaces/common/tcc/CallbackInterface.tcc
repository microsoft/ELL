////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CallbackInterface.tcc (interfaces)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SWIG

// stl
#include <assert.h>
#include <stdexcept>

#endif

namespace ell
{
namespace api
{
    namespace common
    {
        //////////////////////////////////////////////////////////////////////////
        // Api classes for callback forwarding
        //////////////////////////////////////////////////////////////////////////

        template <typename ElementType>
        CallbackForwarder<ElementType>::CallbackForwarder()
            : _buffer(nullptr), _callback(nullptr)
        {
        }

        template <typename ElementType>
        void CallbackForwarder<ElementType>::InitializeOnce(CallbackBase<ElementType>& callback, std::vector<ElementType>& buffer)
        {
            assert(_callback == nullptr);
            assert(_buffer == nullptr);

            if (_callback == nullptr)
            {
                _callback = &callback;
            }

            if (_buffer == nullptr)
            {
                _buffer = &buffer;
            }
        }

        template <typename ElementType>
        bool CallbackForwarder<ElementType>::Invoke(ElementType* buffer)
        {
            if (_callback == nullptr || _buffer == nullptr)
            {
                // Any better exception?
                throw std::invalid_argument("SetCallback has not yet been called");
            }

            bool result = _callback->Run(*_buffer);
            if (result)
            {
                // EFFICIENCY: any way to avoid the copy?
                std::copy(_buffer->begin(), _buffer->end(), buffer);
            }

            return result;
        }
    }
}
}