## Raspberry Pi ELL Python Tutorial

This tutorial shows you how to create a compiled ELL Python module 
that executes a given ELL model on a raspberry pi.


### Hardware / Component Setup

If you want to process video input you will need:

* A camera to capture video, such as a USB webcam or a [Raspberry Pi Camera Module v2](https://www.raspberrypi.org/products/camera-module-v2/). We recommend standard USB webcams because they usually don't require additional driver configuration.
   * If using the Raspberry Pi Camera Module, you will need to enable the V4L driver so that OpenCV can use it. More details are [here](https://www.raspberrypi.org/forums/viewtopic.php?t=62364).

* If you want to see the tagged images on a screen you will need a display to see video, such as an HDMI monitor or a touchscreen display.

You can also process static images, just copy them to the pi.  If your raspberry pi has no screen attached, you can run in headless mode by first setting this variable:

    export DISPLAY=:0

### Software Setup

You will need the following Build pre-requisites:

#### CMake and OpenBLAS:

    sudo apt-get update
    sudo apt-get install -y cmake libopenblas-dev

#### Python 3.4 from Miniconda3. 

The default options should work, although we recommend picking "yes" when you get the prompt to add Miniconda to the PATH in `/home/pi/.bashrc`.

Then from a terminal window at `pi@raspberrypi:~` :

    wget http://repo.continuum.io/miniconda/Miniconda3-latest-Linux-armv7l.sh
    chmod +x Miniconda3-latest-Linux-armv7l.sh
    ./Miniconda3-latest-Linux-armv7l.sh

Follow prompts to install, we recommend [yes] for adding Miniconda3 to the PATH, then run this to create a new conda environment:

    source ~/.bashrc # if you selected [yes] to add to the PATH
    conda create --name py34 python=3

Now each time you log in to your rasperry pi, run this to activate
your new python environment:

    source activate py34

Now you will also need the following Demo pre-requisites:

#### Install NumPy:
    conda install numpy

#### Install the OpenCV

Use the arm7l package from the `microsoft-ell` channel:
```
conda install -c microsoft-ell opencv
```

#### Download an ELL Model

Next we download a CNN model from 
[https://github.com/microsoft/ELL-models](https://github.com/microsoft/ELL-models).

For this tutorial we will use the Darknet Model.

    curl -o darknet.model https://github.com/Microsoft/ELL-models/blob/master/models/darknet/darknet.model 
    curl -o darknet_config.json https://github.com/Microsoft/ELL-models/blob/master/models/darknet/darknet_config.json
    curl -o labels.txt https://github.com/Microsoft/ELL-models/blob/master/models/darknet/labels.txt

#### Compile the Model

Now we generate the project files for compiling this model on the raspberry pi as follows:

    python make_python_project.py darknet_config.json labels.txt darknet.ellmodel -target pi3 -outdir pi3

(make_python_project.py lives in ~/build/tools/project_builder).
This step has actually already compiled the darknet model for your
raspberry pi, you will see it in the object file `darknet.obj`.
This is armv7-linux-gnueabihf code for cortex-a53, so do not try
and link this on Windows.  This is code will get linked into a python module for running on the pi.

So copy the resulting pi3 folder to your raspberry pi.  On Windows
we use WinSCP and just drag/drop the folder, or on Linux you can use 
command line scp on a tar'd version of the folder.

Once there, run this from a Terminal window on the pi:

    mkdir build && cd build
    cmake ..
    make
    cd ..

The build generates a Python ELL module that provides a predict function on a compiled version of the given model.
We have also included a `demo.py` script that loads that module and feeds it video frames which you can run like this:

    python demo.py darknet_config.json labels.txt 

If you have no video camera attached, then you can also classify static images as follows:

    python demo.py darknet_config.json labels.txt schoolbus.png

And if you are running in headless mode you can add the -save argument
to see the resulting classified frame.

    python demo.py darknet_config.json labels.txt schoolbus.png -save

If you have more than one video camera you can provide the camera index:

    python demo.py darknet_config.json labels.txt 2

#### Troubleshooting

##### (frame:2580): Gtk-WARNING **: cannot open display:

You are running in headless mode, so set this variable:

    export DISPLAY=:0

##### ImportError: No module named 'numpy'

You might have forgotten to run `source activate py34`

