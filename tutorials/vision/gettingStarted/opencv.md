## OpenCV

The easiest way to install OpenCV is with your *conda* environment. 
So if you chose to use *miniconda* or *anaconda* then you can simply do this
from your activated py36 environment:

    conda install -c conda-forge opencv

If not, then go to the [opencv.org](http://opencv.org/opencv-3-2.html) website and scroll to the bottom.
You will see various download options, including one for Windows.  

On Linux there are some good instructions here: [Installing OpenCV 3.2.0 for Python 3.5 on Ubuntu 16.04](https://www.begueradj.com/installing-opencv-3.2.0-for-python-3.5.2-on-ubuntu-16.04.2-lts.html).

### Super Advanced Users

If you want to optimize OpenCV for your particular graphics card and get CUDA optimization, you can
clone the opencv git repo, install the NVidia CUDA toolkit, then build the OpenCV sources yourself
and get blazing fast performance.  

Be sure to enable Python3 build of opencv by doing the following:

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

