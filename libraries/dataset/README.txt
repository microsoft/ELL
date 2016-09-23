README.txt for the dataset library

This library implements the ability to load and store data vectors from an input stream. It includes procedures to parse textual dataset representations. It also includes several different in-memory representations of data vectors, and provides an automatic mechanism for choosing the best representation for each instance. 

In-memory representations of data vectors:
==========================================
* IDataVector - IDataVector is an extension of IVector, which is an interface from the 'linear' library. IDataVector provides functions to interface with the feature values of data points. An instance of IDataVector should be thought of as an infinite-dimensional vector of double precision real numbers, which ends with an infinite sequence of zeros, and which can be physically stored in various different ways. Typically, an IDataVector is created once, never modified, and accessed using forward iteration over its non-zero entries.

Types of IDataVectors:
======================
* DenseDoubleDataVector - The prefix of non-zero entries is kept in a std::vector<double> 

* DenseFloatDataVector - The prefix of non-zero entries is kept in a std::vector<float>, but note that the interface always makes the entries of the vector appear to be doubles. The single precision is used only to compress the internal representation, and the values are always cast to double when they are accessed.

* SparseDoubleDataVector - Implemented as a list of index value pairs. The indices are stored in a compressed variable-length delta-encoding (each index can occupy 1-4 bytes). The values are kept in double precision.

* SparseFloatDataVector - Implemented like sparse_double_dataVector, except that values are stored in single precision. The single precision is used just to compress the internal representation, and the values are always cast to double when they are accessed from the outside.

* SparseShortDataVector - Implemented like sparse_double_dataVector, except that values are stored as short integers. The short ints are just to compress the internal representation, and the values are always cast to double when they are accessed from the outside.

* SparseBinaryDataVector - Implemented as a list of indices, which are stored in a compressed variable-length delta-encoding (each index can occupy 1-4 bytes). The non-zero entries of this vector appear as double precision 1.0 when accessed from the outside.

DataVectorBuilder
==================
The user is not expected to manually choose one of the IDataVector implementations. DataVectorBuilder contains simple logic that attempts to find the best representation for each vector. To find out which vector type was chosen, use the interface function IDataVector::GetType().
