# Advanced Installation Topics

## Manually Setting the OpenBLAS Architecture

CMake automatically determines your processor architecture and chooses the right version of OpenBLAS. To override this, use the `-DPROCESSOR_HINT` CMake command line option. For example, for the Intel Haswell processor architecture, add the command line argument `-DPROCESSOR_HINT=haswell`.

## Testing ELL

There are some optional components that you can build and test depending on
whether you have installed these python modules:

    1. CNTK, install using `pip install cntk`
    2. ONNX, install using `conda install -c conda-forge onnx`

If you want to buidl and test these optional components then add the following to your ELL `cmake` command line:

```shell
cmake -DONNX=ON -DCNTK=ON ..
```

The [CTest](https://cmake.org/cmake/help/v3.9/manual/ctest.1.html) tool comes with your CMake installation. To run build tests, type:

```shell
    cd build
    [Linux/macOS] ctest
    [Windows] ctest -C Release
```
If tests fail, add the `-VV` option to get verbose output from each test, to narrow down the problem.  The `-R` option is also handy in this case to run only those tests that have a matching name.

## Generating Code Documentation with Doxygen

To create code documentation using *Doxygen*, build the project named `doc`.

```
cmake --build . --target doc
```

The top level entry point to the documentation will appear in `<ELL_Root>/build/doc/html/index.html`

## OpenCV with CUDA

Advanced users can compile OpenCV with CUDA support. To do so, clone the OpenCV git repository, install the NVidia CUDA toolkit, and build OpenCV with CUDA.

    mkdir build
    cd build

    set PYTHON3_EXECUTABLE=d:\Continuum\Anaconda2.7\envs\py35\python.exe
    set PYTHON3_INCLUDE_DIR=d:\Continuum\Anaconda2.7\envs\py35\include
    set PYTHON3_LIBRARY=d:\Continuum\Anaconda2.7\envs\py35\libs\python35.lib
    set PYTHON3_NUMPY_INCLUDE_DIRS=d:\Continuum\Anaconda2.7\envs\py35\lib\site-packages\numpy\core\include
    set PYTHON3_PACKAGES_PATH=d:\Continuum\Anaconda2.7\envs\py35\lib\site-packages

    cmake  -G "Visual Studio 15 2017 Win64" -D CMAKE_BUILD_TYPE=Release -D WITH_CUDA=1  -D BUILD_TESTS=OFF -D BUILD_PERF_TESTS=OFF ..
    if ERRORLEVEL 1 goto :eof
    cmake --build . --config Release
