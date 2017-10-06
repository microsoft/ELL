## ELL Python Package

This folder contains the scripts needed to build the `conda` package containing the ELL compiler.

After building the Release version of ELL and _ELL_python target, you can run this:

```
conda build --python 3.6 --build-only .
```
