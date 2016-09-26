# Overview of the dataset library design

This library implements the ability to load and store data vectors from an input stream. 
It includes procedures to parse textual dataset representations. 
It also includes various dense and sparse implementations of data vectors, along with an automatic data-dependent mechanism for choosing the best representation. 
A data vector should be thought of as an *infinite-dimensional* vector, whose elements are *double precision* real numbers, and which ends with an infinite sequence of zeros. Typically, a data vector is not modified after its creation and is accessed via forward read-only iteration over its non-zero entries.
All data vectors implement the `IDataVector` interface, which requires the following functions:
* A
* B

Additionaly, all data vectors implement:
* GetIterator
* ctors: Iterator, other DV, init_list
* ToDataVector 

### Implementations of IDataVectors:

* DoubleDataVector - The prefix of non-zero entries is kept in a std::vector<double> 

* FloatDataVector - 

* Short

* Byte

* SparseDoubleDataVector - The prefix of non-zero entries is kept in a std::vector<double> 

* SparseFloatDataVector - 

* SparseShort

* SparseByte

* SparseBinary

