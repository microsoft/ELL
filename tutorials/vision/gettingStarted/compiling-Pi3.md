
### Raspberry Pi 3

This portion of the tutorial presumes you are familiar with setting up a Raspberry Pi. If not, the [official documentation](https://www.raspberrypi.org/documentation/) is a great place to start.

The steps below are tested on a Raspberry Pi 3, running a current version of 32-bit Raspbian.  We'll be testing with other versions of Raspberry Pis and will update the tutorial as and when they are ready.


#### Compiling the Model for Raspberry Pi 3

Similar to the steps for compiling on windows or linux, you can compile
a model for Raspberry Pi by following these steps:

```
## Windows
cd ELL/build
cmake --build . --target compiled_darknetReference_pi3 --config Release

## Linux
cd ELL/build
make compiled_darknetReference_pi3
```

Once the build is complete, copy the  `build/tutorials/vision/gettingStarted/compiled_darknetReference_pi3` folder to the Raspberry Pi 3. If you have [SSH enabled](https://www.raspberrypi.org/documentation/remote-access/ssh/) on the Raspberry Pi 3, you can use [Secure Copy](https://www.raspberrypi.org/documentation/remote-access/ssh/scp.md). 

For example (from Linux / Mac host, or from [BashOnWindows](https://msdn.microsoft.com/en-us/commandline/wsl/install_guide) 
replacing IP_ADDRESS with your Raspberry Pi's IP address):
```
cd ELL/build/tutorials/vision/gettingStarted/compiled_darknetReference_pi3
tar zcvf compiled_darknetReference_pi3.tgz compiled_darknetReference_pi3
scp compiled_darknetReference_pi3.tgz pi@IP_ADDRESS:/home/pi/compiled_darknetReference_pi3.tgz
```

If copying from a Windows system, the [WinSCP](https://winscp.net) application works well. 

#### Hardware / Component Setup

To run the demo, you will need:
* A camera to capture video, such as a USB webcam or a [Raspberry Pi Camera Module v2](https://www.raspberrypi.org/products/camera-module-v2/). We recommend standard USB webcams because they usually don't require additional driver configuration.
   * If using the Raspberry Pi Camera Module, you will need to enable the V4L driver so that OpenCV can use it. More details are [here](https://www.raspberrypi.org/forums/viewtopic.php?t=62364).

* A display to see video, such as an HDMI monitor or a touchscreen display.

#### Software Setup

1. Build pre-requisites:

   * CMake and OpenBLAS:

    ```
    pi@raspberrypi:~ $ sudo apt-get update
    pi@raspberrypi:~ $ sudo apt-get install -y cmake libopenblas-dev
    ```

   * Python 3.4 from Miniconda3. The default options should work, although we recommend picking "yes" when you get the prompt to add Miniconda to the PATH in /home/pi/.bashrc.

    ```
    pi@raspberrypi:~ $ wget http://repo.continuum.io/miniconda/Miniconda3-latest-Linux-armv7l.sh
    pi@raspberrypi:~ $ chmod +x Miniconda3-latest-Linux-armv7l.sh
    pi@raspberrypi:~ $ ./Miniconda3-latest-Linux-armv7l.sh
    ```
    Follow prompts to install, we recommend [yes] for adding Miniconda3 to the PATH.
    ```
    pi@raspberrypi:~ $ source ~/.bashrc # if you selected [yes] to add to the PATH
    pi@raspberrypi:~ $ conda create --name py34 python=3
    pi@raspberrypi:~ $ source activate py34
    ```

2. Demo pre-requisites:

   * Install NumPy:
    ```
    (py34) pi@raspberrypi:~ $ conda install numpy
    ```
   * Install the OpenCV arm7l package from the `microsoft-ell` channel:
    ```
    (py34) pi@raspberrypi:~ $ conda install -c microsoft-ell opencv
    ```

##### Building the compiled Darknet module

If you've copied the compiled model's folder (containing the generated object and wrapper code) to the Raspberry Pi 3, proceed to building the module:

```
pi@raspberrypi:~ $ tar zxvf compiled_darknetReference_pi3.tgz # if you had scp'ed the tarball earlier
pi@raspberrypi:~ $ cd compiled_darknetReference_pi3
pi@raspberrypi:~ $ mkdir build && cd build
pi@raspberrypi:~ $ cmake ..
pi@raspberrypi:~ $ make
```

This will build a `_darknetReference.so` Python module that our compiled demo script will use.

##### Testing the compiled Darknet module

Run the compiled demo script from the `compiled_darknetReference_pi3` folder and a Python 3.4 Miniconda environment:
```
(py34) pi@raspberrypi:~ $ cd compiled_darknetReference_pi3
(py34) pi@raspberrypi:~ $ python compiledDarknetDemo.py
```

As with the non-compiled demo, you can optionally specify a camera number or image filename when you run the demo:
```
    python compiledDarknetDemo.py 1
    python compiledDarknetDemo.py impala.jpg
```
