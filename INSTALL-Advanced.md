# Advanced Installation Topics

## Manually Setting the OpenBLAS Architecture

CMake automatically determines your processor architecture and chooses the right version of OpenBLAS. To override this, use the `-DPROCESSOR_HINT` CMake command line option. For example, for the Intel Haswell processor architecture, add the command line argument `-DPROCESSOR_HINT=haswell`.

## Testing ELL 

The [CTest](https://cmake.org/cmake/help/v3.9/manual/ctest.1.html) tool comes with your CMake installation. To run build tests, type

    cd build
    ctest 

If tests fail, add the `-VV` option to get verbose output from each test, to narrow down the problem.

## Generating Code Documentation with Doxygen

To create code documentation using *Doxygen*, build the project named `doc`. Specifically, if CMake is used to generate a makefile, type `make doc`. If CMake is used to generate a Visual Studio solution, open that solution in Visual Studio, find the project named `doc` in the solution explorer, and build it. The generated files will appear in `ELL/build/doc`.

## OpenCV with CUDA

Advanced users can compile OpenCV with CUDA support. To do so, clone the OpenCV git repository, install the NVidia CUDA toolkit, and build OpenCV with CUDA.

    mkdir build
    cd build

    set PYTHON3_EXECUTABLE=d:\Continuum\Anaconda2.7\envs\py35\python.exe
    set PYTHON3_INCLUDE_DIR=d:\Continuum\Anaconda2.7\envs\py35\include
    set PYTHON3_LIBRARY=d:\Continuum\Anaconda2.7\envs\py35\libs\python35.lib
    set PYTHON3_NUMPY_INCLUDE_DIRS=d:\Continuum\Anaconda2.7\envs\py35\lib\site-packages\numpy\core\include
    set PYTHON3_PACKAGES_PATH=d:\Continuum\Anaconda2.7\envs\py35\lib\site-packages

    cmake  -G "Visual Studio 14 2015 Win64" -D CMAKE_BUILD_TYPE=Release -D WITH_CUDA=1  -D BUILD_TESTS=OFF -D BUILD_PERF_TESTS=OFF ..
    if ERRORLEVEL 1 goto :eof
    cmake --build . --config Release