# Advanced Installation Topics

## Manually Setting the OpenBLAS Architecture

CMake automatically determines your processor architecture and chooses the right version of OpenBLAS. To override this, use the `-DPROCESSOR_HINT` CMake command line option. For example, for the Intel Haswell processor architecture, add the command line argument `-DPROCESSOR_HINT=haswell`.

## Manually building OpenBLAS on Windows

If you find that you have a CPU that is not supported by the versions of OpenBlas that
are included in the nuget package listed in `external/packages.config` then you can build and
install OpenBlas manually by following these steps:
```
git clone https://github.com/xianyi/OpenBLAS
cd OpenBLAS
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 ..
cmake --build . --config Release
```
Then from an `Administrator` command prompt run the following install comamnd, from the above build folder:
```
cmake --build . --config Release --target INSTALL
```
This will put a version of OpenBlas that works for your CPU at `c:\Program Files\OpenBlas`.
Now if you delete your ELL build folder and re-run the cmake step you should see output like this, which confirms that cmake found your new library:
```
-- Found OpenBlas in C:\Program Files/OpenBlas
-- Using BLAS include path: C:/Program Files/OpenBLAS/include
-- Using BLAS library: C:/Program Files/OpenBLAS/lib/libopenblas.lib
```
Lastly, be sure to add the following path to your PATH environment so that the ELL binaries can actually find `libopenblas.dll`:
```
set PATH=%PATH%;c:\Program Files\OpenBlas\bin
```

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

    set PYTHON3_EXECUTABLE=%LOCALAPPDATA%\Continuum\miniconda3\envs\py36\python.exe
    set PYTHON3_INCLUDE_DIR=%LOCALAPPDATA%\Continuum\miniconda3\envs\py36\include
    set PYTHON3_LIBRARY=%LOCALAPPDATA%\Continuum\miniconda3\envs\py36\libs\python35.lib
    set PYTHON3_NUMPY_INCLUDE_DIRS=%LOCALAPPDATA%\Continuum\miniconda3\envs\py36\lib\site-packages\numpy\core\include
    set PYTHON3_PACKAGES_PATH=%LOCALAPPDATA%\Continuum\miniconda3\envs\py36\lib\site-packages

    cmake -G "Visual Studio 16 2019" -A x64 .. -D CMAKE_BUILD_TYPE=Release -D WITH_CUDA=1  -D BUILD_TESTS=OFF -D BUILD_PERF_TESTS=OFF ..
    if ERRORLEVEL 1 goto :eof
    cmake --build . --config Release
