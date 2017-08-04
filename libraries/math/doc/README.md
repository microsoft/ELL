# Design overview of the `math` library
The `math` library provides basic linear algerbra functionality.

## `Vector` classes
Classes declared in `Vector.h` implement the basic functionality of algebraic vectors. Vectors are templated on their element type, which is typically `double` or `float`, and their orientation, which is either `VectorOrientation::row` or `VectorOrientation::column`.

There are three user-facing vector classes:
* A `ConstVectorReference` does not own any memory, and instead references the memory of a `Vector` (see below). `ConstVectorReference` includes functions that do not modify the vector contents, such as `Size` and `operator==`.
* A `VectorReference` still references the memory of a `Vector` and adds functions that modify the vector contents, like `Reset` and `Fill`.
* A `Vector` owns its memory and includes all the functionality of `ConstVectorReference` and `VectorReference`.

Most vector-vector algebraic operations are implemented outside the `Vector` classes, and appear in `VectorOperations.h`.

## `Matrix` classes
Classes declared in `Matrix.h` implement the basic functionality of algebraic matrices. Matrices are templated on their element type, which is typically `double` or `float`, and their layout, which is either `MatrixLayout::rowMajor` or `MatrixLayout::columnMajor`.

There are three user-facing matrix classes, which mirror the three user-facing vector classes:
* `ConstMatrixReference` 
* `MatrixReference`
* `Matrix`

Most algebraic operations involving matrices are implemented outside the `Matrix` classes, and appear in `MatrixOperations.h`.

## `Tensor` classes
Classes declared in `Tensor.h` implement the basic functionality of three-dimensional algebraic matrices. In other libraries, these are sometimes called Cubes, but here we call them Tensors (although the mathematical definition of a Tensor is more broad). Tensors are templated on their element type, which is typically `double` or `float`. They also have three template parameters that control their layout, which take the values
* `Dimension::row`
* `Dimension::column`
* `Dimension::channel`

For example, `Tensor<ElementType, Dimension::channel, Dimension::column, Dimension::row>` is a Tensor whose primary dimension is the channel dimension and its secondary dimension is the column dimension. As above, the three user-facing tensor classes are:

* `ConstTensorReference` 
* `TensorReference`
* `Tensor`

Most algebraic operations involving tensors are implemented outside the `Tensor` classes, and appear in `TensorOperations.h`.

## Operations
As noted above, algebraic operations on vectors, matrices, and tensors appear in the `VectorOperations.h`, `MatrixOperations.h`, and `TensorOperations.h` files. Some of these operations have multiple implementations: a native (built-in) implementation and a BLAS implementation. Typically, the user is unaware of the underlying implementation, and uses commands like `math::Multiply(s, M)` (which scales the matrix `M` by the scalar `s`). If the precompiler macro `USE_BLAS` is defined, this command invokes the BLAS implementation and otherwise it invokes the native implementation.

To explicitly invoke a specific implementation, use `math::Internal::MatrixOperations<math::ImplementationType::native>::Multiply` or `math::Internal::MatrixOperations<math::ImplementationType::openBlas>::Multiply`. If `USE_BLAS` is not defined during compilation, then both of these calls will invoke the native implementation. 
