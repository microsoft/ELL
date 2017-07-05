# Design overview of the `math` library
The `math` library provides basic linear algerbra functionality.

## `Vector` classes
Classes declared in `Vector.h` implement the basic functionality of algebraic vectors. 
Vectors are templated on their element type, which is typically `double` or `float`, and their orientation, which is `VectorOrientation::row` or `VectorOrientation::column`.

There are three user-facing vector classes:
* A `ConstVectorReference` does not own any memory, and instead references the memory of a `Vector` (see below). `ConstVectorReference` includes functions that do not modify the vector contents, such as `Size` and the `operator==`.
* A `VectorReference` extends the functionality of `ConstVectorReference` and still references the memory of a `Vector`. It adds functions that modify the vector contents, like `Reset` and `Fill`.
* A `Vector` owns its memory and includes all the functionality of `ConstVectorReference` and `VectorReference`.

Most algebraic vector operations are implemented outside the `Vector` classes, and appear in `Operations.h`.

## `Matrix` classes
Classes declared in `Matrix.h` implement the basic functionality of algebraic matrices.
Matrices are templated on their element type, which is typically `double` or `float`, and their layout, which is one of the following:
* `MatrixLayout::rowMajor` is a rectangular row-major layout
* `MatrixLayout::columnMajor` is a rectangular column-major layout

There are three user-facing matrix classes, which mirror the three user-facing vector classes:
* `ConstMatrixReference` 
* `MatrixReference`
* `Matrix`

## `Tensor` classes
Classes declared in `Tensor.h` implement the basic functionality of three-dimensional algebraic matrices. In other libraries, these are sometimes called Cubes. Tensors are templated on their element type, which is typically `double` or `float`. They also have three template parameters that control their layout, which can take the values
* `Dimension::row`
* `Dimension::column`
* `Dimension::channel`

For example, `Tensor<ElementType, Dimension::channel, Dimension::column, Dimension::row>` is a Tensor whose primary dimension is the channel dimension and its secondary dimension is the column dimension. As above, the three user-facing tensor classes are:

* `ConstTensorReference` 
* `TensorReference`
* `Tensor`

## Operations
Algebraic operations on vectors and matrices are declared in `Operations.h` and operations on tensors are declared in `TensorOperations.h`. All of these operations have a native (built-in) implementation, and some of them also have an `OpenBLAS` implementation. Typically, the user is unaware of the underlying implementation, and uses commands like `math::Operations::Multiply(s, M)` (which scales the matrix `M` by the scalar `s`). If the precompiler macro `USE_BLAS` is defined, this command invokes the OpenBLAS implementation, and otherwise it invokes the native implementation.

To explicitly invoke a specific implementation, use `math::OperationsImplementation<math::ImplementationType::native>::Multiply` or `math::OperationsImplementation<math::ImplementationType::openBlas>::Multiply`. If `USE_BLAS` is not defined during compilation, then both of these calls will invoke the native implementation. 
