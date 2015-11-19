README.txt for the linear library

This library implements basic dense matrix-vector operations for machine learning applications. 

Basic structure:
================
* IVector - IVector is an interface to real-valued vectors (in the linear algebra sense). Classes that implement this interface can participate in dot products and can be added to other vectors (with some restrictions).

* IMatrix - IMatrix is the interface for real-valued matrices. An IMatrix implements basic matrix-vector multiplication.

IVector types
=============
* double_vector - Basically a wrapper around std::vector<double>

* double_subvector_ref - Implemented as a double* (pointer to some position in a double_vector) and a size.

IMatrix types
=============

* DoubleMatrix<matrixStructure::row> - Implemented as a row-major dense matrix of doubles.

* DoubleMatrix<matrixStructure::column> - Implemented as a column-major dense matrix of doubles.

* DoubleMatrix<matrixStructure::column_square> - Like DoubleMatrix<matrixStructure::row>, with additional functions that apply only to square matrices (not fully implemented yet)

* DoubleMatrix<matrixStructure::row_square> - Like DoubleMatrix<matrixStructure::column>, with additional functions that apply only to square matrices (not fully implemented yet)

* DoubleMatrix<matrixStructure::row_square_uptriangular> - Stores only the upper triangle of a square row-major matrix.

* DoubleMatrix<matrixStructure::diagonal> - Stores only the matrix diagonal.

* RowMatrix - Implemented as a array of row vectors. For example, each row vector could be a sparse_binary_datavector or even a unique_ptr<idatavector>. This implementation is designed for storing datasets.

* BiMatrix - Stores row_matrices, one represents the original matrix and the other represents the matrix transpose (not yet fully implemented)




