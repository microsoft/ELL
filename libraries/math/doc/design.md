# Design overview of the `math` library
The `math` library provides basic math functionality.

## `Vector` classes
Classes declared in `Vector.h` implement the basic functionality of algebraic vectors. 
Vectors are templated on their element type, which is typically `double` or `float`, and their orientation, which is `VectorOrientation::row` or `VectorOrientation::column`.

There are three user-facing vector classes:
* A `ConstVectorReference` does not own any memory, and instead references the memory of another vector. `ConstVectorReference` includes functions that do not modify the vector contents, such as `Size` and the equality operator.
* A `VectorReference` also does not own any memory. In includes all of the functionality of `ConstVectorReference`, plus it includes functions that modify the vector contents, such as `Reset` and `Fill`.
* A `Vector` owns memory, and includes the functionality of `Vector Reference`.

The only non-user-facing vector class is `VectorBase`, which is a helper base class for the other vector classes, and makes it easy for them to know their own orientation. 

Algebraic vector operations are not implemented in the `Vector` class. 
Instead, they are implemented in a separate set of classes, which are declared in `Operations.h`.

## `Matrix` classes
Classes declared in `Matrix.h` implement the basic functionality of algebraic matrices.
Matrices are templated on their element type, which is typically `double` or `float`, and their layout, which is one of the following:
* `MatrixLayout::rowMajor` is a rectangular row-major layout
* `MatrixLayout::columnMajor` is a rectangular column-major layout
* In the future, we may add `symmetric`, `upperTriangular`, and `diagonal` layouts. 

There are three user-facing matrix classes, which mirror the three user-facing vector classes:
* `ConstMatrixReference` 
* `MatrixReference`
* `Matrix`

The non-user-facing classes that help define matrices include:
* `VectorReferenceConstructor`, which is a base class for matrices that enables them to construct vector references (used to get references to matrix rows and columns).
* `RectangularMatrixBase`, which is the base class for rectangular matrices. 
* `MatrixBase`, which is a template class that is specialized on the matrix layout, and defines some layout-specific constants.

## Operations
Algebraic operations on vectors and matrices are declared in `Operations.h`. 
The code design is influenced by the fact that these operations have multiple implementations. 
Currently, the `math` library includes a native (built-in) implementation of all operations, and an `OpenBLAS` implementation of most of the operations. 

Typically, the user is unaware of the underlying implementation, and uses commands such as `math::Operations::Multiply(s, M)` (which scales the matrix `M` by the scalar `s`).
If the precompiler macro `#USE_BLAS` is defined, this command invokes the OpenBLAS implementation, and otherwise it invokes the native implementation.
To invoke a specific implementation, use `math::OperationsImplementation<math::ImplementationType::native>::Multiply` or `math::OperationsImplementation<math::ImplementationType::openBlas>::Multiply`.
If `#USE_BLAS` is not defined during compilation, then both of these calls will invoke the native implementation. 

Under the hood, the different vector and matrix operations are implemented in the following classes:
* `CommonOperations`, which is a non-user-facing class that contains operations that only have a native implementation and are not included in the BLAS specification (for example, computing the zero-norm of a vector).
* `DerivedOperations`, which is a non-user-facing class that contains operations that are derived from other operations (for example, scaling a matrix by a scalar, which is implemented as either a row-by-row or column-by-column scaling).
* `OperationsImplementation`, which is specialized on the implementation type and include all of the implementation specific code.
