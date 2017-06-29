# Overview of the data library design

This library implements the following:
* Procedures for parsing textual dataset representations from an input stream.
* The concept of a `DataVector`, which is a mathematical vector specialized for storing data. 
* Various memory representations (dense, sparse, etc.) of a data vector, including an automatic mechanism for choosing the best representation per instance.
* Basic linear operators between a data vector and a `math::Vector`.

## The `IDataVector` interface
A `DataVector` is a sequence of *double precision* real numbers. A data vector contains a finite number of non-zero elements, but we think of it as an infinite sequence that ends with a suffix of zeros. Internally, a `DataVector` may be stored using floats, integers, or even single bits, yet externally it always presents itself as a sequence of doubles. Typically, a data vector is not modified after its creation and is accessed via forward read-only iteration over its elements. 

All data vectors implement the `IDataVector` interface. The main functions defined in this interface are:

* `GetType()` - reveals the internal representation used to store the data vector.
* `PrefixLength()` - as mentioned above, every data vector ends with an infinte suffix of zeros; this function returns the index of the first zero in this suffix. 
* `ToArray()` - Copies the contents of this data vector into a `std::vector<double>` of size `PrefixLength()`
* `CopyAs<DataVectorType>()` - Copies the contents of this data vector to a specific data vector implementation.
* `Print()` - generates a human readable representation of the data vector
* Basic linear operations, such as `Norm2Squared()`, `Dot()`, `AddTo()`. The last two functions also take a `math::Vector`.

## DataVector implementations
The `data` library implements the following representations of a data vector

* `DoubleDataVector` - The prefix of non-zero entries is kept in a `std::vector<double>`
* `FloatDataVector` - The prefix of non-zero entries is kept in a `std::vector<float>`
* `ShortDataVector` - The prefix of non-zero entries is kept in a `std::vector<short>`
* `ByteDataVector` - The prefix of non-zero entries is kept in a `std::vector<char>`
* `SparseDoubleDataVector` - The prefix of non-zero entries is kept in an index-value pair representations, where the values are stored as `double`
* `SparseFloatDataVector` - The prefix of non-zero entries is kept in an index-value pair representations, where the values are stored as `float`
* `SparseShortDataVector` - The prefix of non-zero entries is kept in an index-value pair representations, where the values are stored as `short`
* `SparseByteDataVector` - The prefix of non-zero entries is kept in an index-value pair representations, where the values are stored as `char`
* `SparseBinaryDataVector` - The prefix of non-zero entries is stored as a list of indices. 
* `AutoDataVector` - This is a special data vector type that internally can be any one of the above, and which implements an automatic mechanism to choose the best representation for a given instance.

## Operations with `math::Vector`
Basic mathematical operations can be performed with `math::Vector`. For example, adding a data vector to a vector

    math::RowVector<double> v(20);
    data::AutoDataVector u{ 0, 0, 1, 0, 1};
    v += u;

Note that the `math::Vector` has a size of 20, while the data vector has a logical size of infinity. This operation requires that the size of the vector is bigger than the index of the last non-zero entry in the data vector.

Other operations include:

    v += -3.2 * u;
    v += Square(u);
    v += Sqrt(u);
    v += Abs(u);

