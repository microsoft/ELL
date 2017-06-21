# ELL Language Binding Examples

## Overview

This section covers how to generate and test the language bindings for emitted ELL models. Emitted ELL models are compiled binary code with language-specific wrappers for their evaluation functions.

## General setup

Requires SWIG and Python 3
* For instructions on installing SWIG and Python, refer to [this section](../../../interfaces/README.md)

## Examples

Below is a list of available examples:

* step10 - Demonstrates calling step on a simple times two model, using a callback to supply the 10 input values.


## Python

1. Build the generated python bindings. This will also run llc to compile the LLVM IR into an .o for your developer system.

Linux / Mac:
```
make _ELL_step10
```
Windows:
```
build the examples/_ELL_step10 project
```

2. Test the generated python bindings.

Linux / Mac:
```
source activate py36
ctest -R ELL_step10_python_test -VV
```
Windows:
```
activate py36
ctest -R ELL_step10_python_test -VV -C Debug
```
