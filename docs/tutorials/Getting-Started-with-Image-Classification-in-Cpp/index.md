---
layout: default
title: Getting started with image classification on the Raspberry Pi in C++
permalink: /tutorials/Getting-started-with-image-classification-in-cpp/
---
# Getting started with image classification on Raspberry Pi in C++

*by Byron Changuion and Ofer Dekel*

In this tutorial, we will download a pretrained image classifier from the [ELL gallery](/ELL/gallery/) to a laptop or desktop computer. We will then compile the classifier as a C++ object file and create a CMake project that makes it easy to use the classifier in a C++ program. Finally, we will write a simple C++ program that captures images from the Raspberry Pi's camera and classifies them.

---

![screenshot](/ELL/tutorials/Getting-Started-with-Image-Classification-in-Cpp/Screenshot.png)

#### Materials

* Laptop or desktop computer
* Raspberry Pi
* Raspberry Pi camera or USB webcam
* *optional* - Active cooling attachment (see our [tutorial on cooling your Pi](/ELL/tutorials/Active-cooling-your-Raspberry-Pi-3/))

#### Prerequisites

* Install ELL on your computer ([Windows](https://github.com/Microsoft/ELL/blob/master/INSTALL-Windows.md), [Ubuntu Linux](https://github.com/Microsoft/ELL/blob/master/INSTALL-Ubuntu.md), [Mac](https://github.com/Microsoft/ELL/blob/master/INSTALL-Mac.md)). Specifically, this tutorial requires ELL, CMake, and Python 3.6. Note that Python is required to run a tool named `wrap.py`, which makes compilation easy. If you prefer not to use `wrap.py`, you can perform the compilation steps manually, as described in the [wrap tool documentation](https://github.com/Microsoft/ELL/blob/master/tools/wrap/README.md).
* Follow the instructions for [setting up your Raspberry Pi](/ELL/tutorials/Setting-up-your-Raspberry-Pi).
* *optional* - Read through the instructions in [Getting started with image classification on the Raspberry Pi](/ELL/tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/).

## Step 1: Create a tutorial directory

Change to the directory where you built ELL and create a `tutorials` directory. In that directory, create another directory named `cppTutorial`.

```
cd ELL/build
mkdir tutorials
cd tutorials
mkdir cppTutorial
cd cppTutorial
```

## Step 2: Download pre-trained model

Download this [compressed ELL model file](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3CMCMCMCMCMCMC1A/d_I224x224x3CMCMCMCMCMCMC1A.ell.zip) into the `tutorial1` directory. The model file contains a pre-trained Deep Neural Network for image classification, and is one of the models available from the [ELL gallery](/ELL/gallery). The file's long name indicates the Neural Network's architecture, but don't worry about that for now and save it locally as `model.ell.zip`.

```
curl --location -o model.ell.zip https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3CMCMCMCMCMCMC1A/d_I224x224x3CMCMCMCMCMCMC1A.ell.zip
```

Unzip the compressed file and rename the `d_I224x224x3CMCMCMCMCMCMC1A.ell` model file to `model.ell`:

```
[Unix] unzip model.ell.zip` <br> `mv d_I224x224x3CMCMCMCMCMCMC1A.ell model.ell
[Windows] unzip model.ell.zip` <br> `ren d_I224x224x3CMCMCMCMCMCMC1A.ell model.ell
```

(One Windows, unzip is part of the Git distribution, for example, in `\Program Files\Git\usr\bin`.)
Next, download the file of [category names](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/categories.txt) that correspond to this model into the `cppTutorial` directory.

```
curl --location -o categories.txt https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/categories.txt
```

There should now be a `model.ell` file and a `categories.txt` file in the `cppTutorial` directory.

## Step 3: Compile the model for static linking into a C++ program

Deploying an ELL model to the Raspberry Pi using C++ requires two steps. First, the ELL compiler compiles `model.ell` into machine code. Next, we make all the preparations needed to wrap the compiled model in a CMake project. The result of this step is a special CMake project that contains all of the configurations and settings needed to link the compiled ELL model into a C++ project.

These steps are performed by a handy tool named `wrap`. Run `wrap` as follows.

```
python "../../tools/wrap/wrap.py" categories.txt model.ell -lang cpp -target pi3 -outdir model
```

Note that we gave `wrap` the command line option `-target pi3`, which tells it to generate machine code for execution on the Raspberry Pi. We also used the `-outdir model` option to tell `wrap` to put the all output files into a directory named `model`. If all goes well, you should see the following output.

```
    compiling model...
    running llc...
    success, now you can build the 'model' folder
```

The `wrap` tool creates a `cmake` project in a new directory named `model`. The `model` directory contains an object file with the compiled model and a header file that defines the following static functions.
    * model_predict - The model's main function for classifying images
    * model_GetInputShape - gets the shape of the input expected by the model
    * model_GetOutputShape - get the shape of the output generated by the model
    * A CMakeLists.txt file that defines a CMake project, which make it easy to link the model with other CMake projects.

Copy over some additional C++ helper code that makes it easier to send images to the model.

```
[Unix] cp ../../../docs/tutorials/Getting-Started-with-Image-Classification-in-Cpp/*.h .
[Windows] copy ..\..\..\docs\tutorials\Getting-Started-with-Image-Classification-in-Cpp\*.h .
```

A this point, you should now have a `cppTutorial` directory that contains helper C++ code, a `model` subdirectory with the compiled ELL model and a CMake project.

## Step 4: Call your model from a C++ program

We will write a C++ program that invokes the model and run the demo on a Raspberry Pi. The program will read images from the camera, pass them to the model, and display the results. Either copy the complete code from [here](/ELL/tutorials/Getting-Started-with-Image-Classification-in-Cpp/tutorial.cpp) or create an empty text file named `tutorial.cpp` in the `cppTutorial` directory and copy in the code snippets below.

First, add the required include statements. Our code depends on some STL libraries and on the OpenCV library.

```cpp
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <opencv2/opencv.hpp>
```

Also include the header file that was generated by ELL when compiling the model.

```cpp
#include "model.i.h"
```

Finally, include the helper functions that were copied over earlier.

```cpp
#include "tutorialHelpers.h"
```

Define the following functions, which will help us get images from the camera and read in the categories file.

```cpp
// Read an image from the camera
static cv::Mat GetImageFromCamera(cv::VideoCapture& camera)
{
    cv::Mat frame;
    camera >> frame;
    return frame;
}

// Read a file of strings
static std::vector<std::string> ReadLinesFromFile(const std::string& filename)
{
    std::vector<std::string> categories;

    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line))
    {
        if (line.length() > 0) categories.emplace_back(line);
    }

    return categories;
}
```

Define the `main` function and start the camera.

```cpp
int main(int argc, char** argv )
{
    cv::VideoCapture camera(0);
```

Use the function we defined above to read the category names from the file provided on the command line.

```cpp
    auto categories = ReadLinesFromFile("categories.txt");
```

The model expects its input in a certain shape. Get this shape and store it for use later on.  

```cpp
    TensorShape inputShape;
    model_GetInputShape(0, &inputShape);
```

Allocate a vector to store the model's output. 

```cpp
    std::vector<float> predictions(model_GetOutputSize());
```

Next, set up a loop that keeps going until OpenCV indicates it is done, which is when the user hits any key. At the start of every loop iteration, read an image from the camera:

```cpp:
    while (cv::waitKey(1) == 0xFF)
    {
        cv::Mat image = GetImageFromCamera(camera);
```

The image stored in the `image` variable cannot be sent to the model as-is, because the model takes its input as an vector of `float` values. Moreover, the model expects the input image to have a certain shape and a specific ordering of the color channels (which, in this case, is Blue-Green-Red). Since preparing images for the model is such a common operation, we created a helper function for it named `PrepareImageForModel`.

```cpp
        auto input = tutorialHelpers::PrepareImageForModel(image, inputShape.columns, inputShape.rows);
```

With the processed image input handy, call the `predict` method to invoke the model.

```cpp
        model_predict(input, predictions);
```

The `predict` method fills the `predictions` vector with the model output. Each element of this array corresponds to one of the 1000 image classes recognized by the model. Extract the top 5 predicted categories by calling the helper function `GetTopN`.

```cpp
        auto top5 = tutorialHelpers::GetTopN(predictions, 5);
```

Match the category indices in `top5` with the category names in `categories`.

```cpp
        std::stringstream headerText;
        for (auto element : top5)
        {
            headerText << categories[element.first] << " (" << std::floor(element.second * 100.0) << "%) ";
        }
```

Use the `DrawHeader` helper function to display the predicted category on the Raspberry Pi's display. Also display the camera image.

```cpp
        tutorialHelpers::DrawHeader(image, headerText.str());
        cv::imshow("ELL model", image);
    };
    return 0;
}
```

## Step 5: Create a CMake project for your program

We will use CMake to build our project. Either download the complete `CMakeLists.txt` from [here](/ELL/tutorials/Getting-Started-with-Image-Classification-in-Cpp/CMakeLists.txt), or create an empty text file named `CMakeLists.txt` and copy in the project definitions below.

```cmake
project( tutorial )
```

Set the `OpenCV_DIR` variable to your `OpenCV/build` location. For example, if the full path to that directory is `C:/Install/OpenCV3/opencv/build` then add the following. 

```cmake
set(OpenCV_DIR C:/Install/OpenCV3/opencv/build)
```

Tell CMake to find OpenCV.

```cmake
find_package(OpenCV REQUIRED)
```

Next, tell CMake to add your compiled ELL model.

```cmake
add_subdirectory(model)
```

Define the target executable `tutorial` and tell CMake to build `tutorial.cpp` as part of it.

```cmake
add_executable(tutorial tutorial.cpp)
```

Finally, tell CMake to link the program with OpenCV and the ELL model.

```cmake
target_link_libraries(tutorial ${OpenCV_LIBS} model)
```

We are ready to move to the Raspberry Pi. If your Pi is accessible over the network, you can copy the `cppTutorial` directory using the Unix `scp` tool or the Windows [WinSCP](https://winscp.net/eng/index.php) tool. Be sure to copy the `cppTutorial/model` directory too.

## Step 6: Build the project on the Raspberry Pi

Log into your Raspberry Pi, either remotely using SSH or directly if you have a keyboard and screen attached. Find the `cppTutorial` directory you just copied over and build the CMake project.

```
cd cppTutorial
mkdir build
cd build
cmake ..
make
cd ..
```

## Step 7: Classify live video on the Raspberry Pi

Make sure that a camera is connected to your Pi and run the `tutorial` program. 

You should see a window similar to the screenshot at the top of this page. Point your camera at different objects and see how the model classifies them. Look at `categories.txt` to see which categories the model is trained to recognize and try to show those objects to the model. For quick experimentation, point the camera to your computer screen and have your computer display images of different objects. For example, experiment with different dog breeds and other types of animals.

If you copied the full `tutorial.cpp` file from [here](/ELL/tutorials/Getting-Started-with-Image-Classification-in-Cpp/tutorial.cpp), you will also see the average time in milliseconds it takes the model to process a frame.

## Next steps

The [ELL gallery](/ELL/gallery/) offers different models for image classification. Some are slow and accurate, while others are faster and less accurate. Different models can even lead to different power draw on the Raspberry Pi. Repeat the steps above with different models.

## Troubleshooting

If you run into trouble, you can find some troubleshooting instructions at the bottom of the [Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-up-your-Raspberry-Pi).
