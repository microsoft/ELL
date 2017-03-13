# Compiling nodes as functions

Function signature: 
```
void f(in1, in2, <state>, out1, out2);
```

Emitting code for example function f(x) -> x^2

Normal (inline):
```
Compile()
{
    auto x = InputPort.Load();
    auto x2 = mul(x,x);
    auto out = OutputPort.GetVar();
    out.Set(x2);
}
```

As function:

```
Compile()
{
    if(!function defined)
    {
        emitter.Define f(double[] input, double[] output) 
        {
            auto x = in.Load(); // (?)
            auto x2 = mul(x,x);
            output.Set(x2);
        }
    }

    emitter.Call(f, InputPort, OutputPort);

}
```



