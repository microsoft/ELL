# Value library ("Code Nodes")

## Motivation
Initially, the biggest motivator behind this library is the fact that implementing
a new algorithm takes a lot of time and effort. We also have two
different implementations that we end up writing. One that gets used during the
`Compute()` ("compile-time") phase and one that gets used during the
`Compile()` ("run-time") phase. This is no longer the sole motivation behind
this library -- indeed, there are a number of other discovered use cases.

This work seeks to unify the methods of implementing algorithms while also
keeping the implementations themselves simple.

Other motivations driving this design include keeping the classes and
functions simple and clear in their naming as well as retaining the debugging
experience that's found in the compile-time implementations. Additionally, the
separation of algorithm from emitter allows us to plug emitters of different
types (i.e., GPU compute), while leaving the fundamental algorithm unchanged.

## Namespace

The classes and functions described in this document will live in the
`ell::value` namespace. It is the intention that during usage, the
whole namespace be brought into scope with a `using namespace ` declaration.
This is to make the implementation are clear and concise as possible without
littering it with namespace-level qualifications. NB: this may not be necessary
due to ADL, depending on what one is trying to do.

As this API will be type-erased (more below), usage of this API does not need
to be templated and thus does not need to be in `.h`/`.tcc` files.

## Classes
* `Value` - top-level type-erased class that will be the basis of all
  implementations
  * Can accept
    * `llvm::Value*`
    * scalar values of type `T`, where `T` is
      * `bool`
      * `int`
      * `int64_t`
      * `float`
      * `double`
    * `T*` values, where `T` matches the list above
    * Container types with type `T`, where `T` matches the list above
      * `std::vector<T>`
      * `std::initializer_list<T>`
      * `math::Vector<T>`
      * `math::Matrix<T>`
      * `math::Tensor<T>`
  * When passed in with `llvm::Value*`, `T*`, or `std` containers of `T`, can
    be provided with a `MemoryLayout` to describe memory layout.
  * Provides friendly type introspection functions, meant to be used mostly by
    helper top-level functions (such as the ones below). This works even if
    passed in a `llvm::Value*`; we will introspect into the `llvm` layer as
    necessary.
    * `IsBool` - returns `true` if type is `bool`
    * `IsIntegral` - returns `true` if type is one of the integral types
    * `IsFloating` - returns `true` if `IsIntegral` is `false`
    * `IsConstant` - returns `true` if instance was initialized by anything
      other than `llvm::Value*`
    * `IsGlobal` - returns `true` if value is supposed to have global scope
    * More to come as need arises
  * `GetLayout` - returns an instance of `MemoryLayout`
  * `(...)` - provides an index into the memory layout, returning a `Value`. Out
    of bounds index checking is performed to validate the index, assuming
    index is known at compile-time.
* `Context`
  * Maintains list of "globals"
  * Holds the  necessary information to emit LLVM IR (`IRModuleEmitter`,
    `IRFunctionEmitter`, etc.)
  * Holds a scope name, which is necessary for naming globals.
* `ContextGuard` - RAII helper to push and pop the context based on scope

## Top-level free functions
* `Allocate` - Allocates a local variable
* `GlobalAllocate` - Allocates the `Value` instance with the top-level `Context`
* `For`
* `If`
* `While`
* `Select` - Because C++ doesn't allow you to overload ?:
* `Cast` - Does a type cast of the instance from one `T` to another
  return it
* Math related functions
  * `Abs`
  * `Exp`
  * `GEMM`
  * `GEMV`
  * `Sqrt`
  * `Sum`
  * `Tanh`
  * etc.

## Node implementation usage
* `CompilableCodeNode`, inherits from `model::CompilableNode`
  * `virtual std::vector<value::Value> Define(std::vector<value::Value>) = 0;`
  * `void Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) final;`
  * `void Compute() const final;`
  * `Compute()` and `Compile()` both call into `Define()`
    * prologue:
      * set up relevant context
      * convert input ports to `Value` instances
    * epilogue:
      * convert returned `Value` instances to output port values
      * pop context
* "Code Nodes", inherit from `CompilableCodeNode`
  * just has to provide a definition for `std::vector<value::Value> Define(std::vector<value::Value>)`. `y = f(x)` is the model we're after.
  * Complicated code that needs to do type introspection of `Value` instances
    probably belongs in `value` namespace
  * Example:
    ```cpp
    // SqrtCodeNode.h

    class SqrtCodeNode : public CompilableCodeNode
    {
    public:
        model::InputPortBase input;
        model::OutputPortBase& output;

        SqrtCodeNode();

        SqrtCodeNode(const model::PortElementBase& input);

        std::vector<Value> Define(std::vector<Value>) override;
    };

    // SqrtCodeNode.cpp

    // forward declaration
    Scalar Sqrt(Scalar);

    SqrtCodeNode::L2NormSquaredNode()
        : Node({ &input }, { &output }), input(this, {}, defaultInputPortName), output(this, defaultOutputPortName, 1)
    {
    }

    SqrtCodeNode::SqrtCodeNode(const model::PortElements<ValueType>& input)
        : Node({ &input }, { &output }), input(this, input, defaultInputPortName), output(this, defaultOutputPortName, 1)
    {
    }

    std::vector<Value> SqrtCodeNode::Define(std::vector<Value> input)
    {
      return { Sqrt(input[0]) };
    }
    ```
    Internally, `Sqrt` introspects into the `Value` instance and does a square
    root call. If the value that's held by the `Value` instance is known at
    compile time, this is as simple as calling `std::sqrt` for every element.
    If it is not known at compile time, a for loop is emitted, in which a call
    to `sqrt` is made for each element.
* Converting an existing algorithm:
  Here we look at `Convolve1DSimple` and `Convolve2DSimple` from the `dsp`
  library's SimpleConvolution files. The existing `Convolve1DSimple` function
  looks like (modified for clarity):
  ```cpp
    template <typename ValueType>
    math::RowVector<ValueType> Convolve1DSimple(const math::RowVector<ValueType>& signal, const math::RowVector<ValueType>& filter)
    {
        auto filterSize = filter.Size();
        auto outputSize = signal.Size() - filterSize + 1;
        math::RowVector<ValueType> result(outputSize);

        for (int index = 0; index < outputSize; ++index)
        {
            ValueType accum = 0;
            for (int filterIndex = 0; filterIndex < filterSize; ++filterIndex)
            {
                accum += filter[filterIndex] * signal[index + filterIndex];
            }
            result[index] = accum;
        }
        return result;
    }
  ```
  The version that uses the `Value` infrastructure looks like:
  ```cpp
    Value testConvolve1D(Value signalVar, Value filterVar)
    {
        Vector signal(signalVar), filter(filterVar);

        size_t resultSize = signal.Size() - filter.Size() + 1;
        Vector result(Allocate(signalVar.Type(), resultSize));

        For(result, [&](int index)
        {
            Scalar accum;
            For(filter, [&](int filterIndex)
            {
                accum += filter(filterIndex) * signal(index + filterIndex);
            });

            result(index) = accum;
        });

        return result;
    }
  ```
  It should be noted that in practice, the function above would take in a
  couple of `Vector` instances and return another `Vector` instance. This is
  not done only to explicitly showcase how interactions between `Value` and
  `Vector` would work.
  ``Convolve2DSimple` similarly looks like:
  ```cpp
    template <typename ValueType>
    math::ChannelColumnRowTensor<ValueType> Convolve2DSimple(const math::ConstChannelColumnRowTensorReference<ValueType>& signal, const math::ConstChannelColumnRowTensorReference<ValueType>& filters, int numFilters, int stride)
    {
        const auto filterRows = filters.NumRows() / numFilters;
        const auto filterColumns = filters.NumColumns();
        const auto inputRows = signal.NumRows();
        const auto inputColumns = signal.NumColumns();
        const auto outputRows = (inputRows - filterRows + 1) / stride;
        const auto outputColumns = (inputColumns - filterColumns + 1) / stride;
        math::ChannelColumnRowTensor<ValueType> result(outputRows, outputColumns, numFilters);

        const auto numChannels = static_cast<int>(signal.NumChannels());
        const auto numFilterChannels = static_cast<int>(filters.NumChannels());

        for (int filterIndex = 0; filterIndex < numFilters; ++filterIndex)
        {
            const int channelStart = (filterIndex * numFilterChannels) % numChannels;
            const auto filterOffset = filterIndex * filterRows;
            for (int rowIndex = 0; rowIndex < outputRows; ++rowIndex)
            {
                for (int columnIndex = 0; columnIndex < outputColumns; ++columnIndex)
                {
                    const int inputRowIndex = rowIndex * stride;
                    const int inputColumnIndex = columnIndex * stride;
                    ValueType accum = 0;
                    for (int filterRowIndex = 0; filterRowIndex < filterRows; ++filterRowIndex)
                    {
                        for (int filterColumnIndex = 0; filterColumnIndex < filterColumns; ++filterColumnIndex)
                        {
                            auto signalVector = signal
                                                  .GetSubTensor(inputRowIndex + filterRowIndex, inputColumnIndex + filterColumnIndex, channelStart, 1, 1, numFilterChannels)
                                                  .template GetSlice<math::Dimension::channel>(0, 0);
                            auto filterVector = filters
                                                  .template GetSlice<math::Dimension::channel>(filterOffset + filterRowIndex, filterColumnIndex);
                            accum += math::Dot(signalVector, filterVector);
                        }
                    }
                    result(rowIndex, columnIndex, filterIndex) = accum;
                }
            }
        }

        return result;
    }
  ```
  The version that uses the `Value` infrastructure looks like:
  ```cpp
    Value testConvolve2D(Value signalVar, Value filtersVar, int numFilters, int stride)
    {
        Tensor signal(signalVar), filters(filtersVar);

        const auto filterRows = filters.NumRows() / numFilters;
        const auto filterColumns = filters.NumColumns();
        const auto inputRows = signal.NumRows();
        const auto inputColumns = signal.NumColumns();
        const auto outputRows = static_cast<int>(inputRows - filterRows + 1) / stride;
        const auto outputColumns = static_cast<int>(inputColumns - filterColumns + 1) / stride;
        const auto numChannels = signal.NumChannels();
        const auto numFilterChannels = filters.NumChannels();

        Tensor result(Allocate(signalVar.Type(), MemoryLayout({outputRows, outputColumns, numFilters})));

        for (int filterIndex = 0; filterIndex < numFilters; ++filterIndex)
        {
            Tensor filter = filters.SubTensor(filterIndex * filterRows, 0, 0, filterRows, filterColumns, numFilterChannels);
            const int channelStart = (filterIndex * numFilterChannels) % numChannels;

            // Only used for iterating. Could also construct a memory layout that behaves similarly
            Matrix outputMatrix = result.Slice(slice, slice, 0);
            For(outputMatrix, [&](int rowIndex, int columnIndex)
            {
                const int inputRowIndex = rowIndex * stride;
                const int inputColumnIndex = columnIndex * stride;

                Scalar accum;
                For(filter.Slice(slice, slice, 0), [&](int filterRowIndex, int filterColumnIndex)
                {
                    auto signalVector = signal
                                            .SubTensor(inputRowIndex + filterRowIndex, inputColumnIndex + filterColumnIndex, channelStart, 1, 1, numFilterChannels)
                                            .Slice(0, 0, slice);
                    auto filterVector = filter.Slice(filterRowIndex, filterColumnIndex, slice);
                    accum += Dot(signalVector, filterVector);
                });
                result(rowIndex, columnIndex, filterIndex) = accum;
            });
        }

        return result;
    }
  ```
* `Map::Compute` -- needs to set up global context for compute
* `MapCompiler::CompileMap` -- needs to set up global context for compile

## Other ideas considered
A solution that relies entirely on JITing was considered, however due to
the difficulty this poses during debugging, it was quickly disregarded.

## Integration plans with the rest of ELL
### Ports and PortElements
`Value` will take the place of `PortElement` and `PortElements` within
`InputPort` and `OutputPort`. Furthermore, in the future, there will only be
`Port`, with no distinction between a type for inputs and outputs.

This will allow us to create functions that can be composed yet still create
`Node` links that retain the ability to be further optimized.

Hand-wavy prototype code:
```cpp
auto m = Model();
auto in1 = m.InputNode();
auto in2 = m.InputNode();
auto plus = Plus(in1, in2);
auto sqrt = Sqrt(plus);
m.Save();
```

Each of these function calls would create and hook up nodes like we do
currently, but are a lot simpler and clearer to use.

### Math
There is a desire to keep the `math` library self-contained so that it may be,
in the future, extracted out of ELL and made into its own library. To that end,
the `math` library will remain untouched by this effort. Instead, the `Value`
infrastructure will provide the ability to work with containers from the `math`
library seamlessly.

There is some concern about the duplication of the API, but that cannot be
avoided due to the completely different semantics of the two efforts. Also,
not all of the API surface of the `math` library needs to be mirrored within
the `Value` infrastructure, and vice versa. This also helps keep the two
libraries focused and clean.

In principal, this should work well since not all applications of the `math`
library need to be emittable. From the perspective of the ELL compiler, the
data represented by the `math` library is constant data and can be modeled
seamlessly like we do for other constant data containers (such as
`std::vector<T>`).

### Data
The `data` library could be converted to use `Value` under the hood. This would
allow ELL to deal with data loading and writing within emitted code.

At this point, it is not certain whether this is a desired goal. Such a goal
would also prevent the `data` library from being self-contained, like the
`math` library.

## Limitations
### Debugging
While this approach is certainly more debuggable than if we were to just JIT
the compiled code, it is not as easy or straightforward as using standard C++
containers. This negative experience can be mitigated by the introduction of
special helper files that instruct the debugger on how to treat and display
instances of `Value`.

### Implementations of lower level functions
`Value` and the related work is, ultimately, an abstraction. As such, we are
pushing further down the implementation details. Implementing a `Sqrt`
function, for example, would involve introspecting into the type of value
contained within `Value`, handling it in a number of ways. This can make
implementations somewhat complicated to write. The trade off is that using
these abstractions is simpler and more straightforward. There are also a
limited set of lower-level functions that need to be implemented in this
manner; the rest of the functions can be composed of calls to already
implemented functions.

### Potential loss of node optimization capabilities
As abstractions get better, it will be very simple to call and use relatively
complicated functions. However, this would result in nodes not being generated,
which prevents our node-optimization infrastructure from making optimizations.

This is addressed with the future plan of using `Value` to power `Port` instances
and making the creation and usage of `Node` instances be more functional and
simple.
