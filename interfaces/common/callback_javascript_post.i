////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     callback_javascript_post.i (common)
//  Authors:  Piali Choudhury (pialic)
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#include <functional>
%}

%{
    //
    // Callback wrapper types
    //
    class CallbackBase
    {
    public:
        CallbackBase(){}

        CallbackBase(Nan::Callback* callback) : _callback(callback)
        {};

        Nan::Callback* GetFunction() { return _callback; }

        void Call()
        {
            _callback->Call(0, nullptr);
        }

    protected:
        Nan::Callback* _callback;
    };

    class Callback : public CallbackBase
    {
    public:
        Callback(){}

        Callback(Nan::Callback* callback) : CallbackBase(callback)
        {};

        void Call()
        {
            _callback->Call(0, nullptr);
        }
    };

    class Callback2Int : public CallbackBase
    {
    public:
        Callback2Int() {}

        Callback2Int(Nan::Callback* callback) : CallbackBase(callback) {};

        void Call(int arg1, int arg2)
        {
            Nan::HandleScope scope;
            v8::Handle<v8::Value> args[2];
            args[0] = Nan::New<v8::Number>(arg1);
            args[1] = Nan::New<v8::Number>(arg2);
            _callback->Call(2, args);
        }
    };

    class Callback2Int1Double : public CallbackBase
    {
    public:
        Callback2Int1Double() {}

        Callback2Int1Double(Nan::Callback* callback) : CallbackBase(callback) {};

        void Call(int i, int n, double arg)
        {
            Nan::HandleScope scope;
            v8::Handle<v8::Value> args[3];
            args[0] = Nan::New<v8::Number>(i);
            args[1] = Nan::New<v8::Number>(n);
            args[2] = Nan::New<v8::Number>(arg);
            _callback->Call(3, args);
        }
    };
%}

%typemap(in) Callback
{
    if($input->IsFunction())
    {
        auto func = new Nan::Callback(v8::Local<v8::Function>::Cast($input));
        $1 = Callback(func);
    }
    else
    {
        SWIG_exception_fail(SWIG_ERROR, "$input is not a function");        
    }
}

%typemap(in) Callback2Int
{
    if($input->IsFunction())
    {
        auto func = new Nan::Callback(v8::Local<v8::Function>::Cast($input));
        $1 = Callback2Int(func);
    }
    else
    {
        SWIG_exception_fail(SWIG_ERROR, "$input is not a function");        
    }
}

%typemap(in) Callback2Int1Double
{
    if($input->IsFunction())
    {
        auto func = new Nan::Callback(v8::Local<v8::Function>::Cast($input));
        $1 = Callback2Int1Double(func);
    }
    else
    {
        SWIG_exception_fail(SWIG_ERROR, "$input is not a function");        
    }
}

%typemap(in) v8::Local<v8::Value>
{
    $1 = $input;
}
