---
layout: default
title: Comparing Image Classification models side by side on the Raspberry Pi
permalink: /tutorials/Comparing-Image-Classification-models-side-by-side-on-the-Raspberry-Pi/
---
# Comparing Image Classification models side by side on the Raspberry Pi

![screenshot](/ELL/tutorials/Comparing-Image-Classification-models-side-by-side-on-the-Raspberry-Pi/Screenshot.jpg)

### Materials

* A Raspberry Pi
* Cooling fan attachment (see our [instructions on cooling your Pi](/ELL/gallery/Raspberry-Pi-3-Fan-Mount)) (optional)
* A USB webcam (optional)
* A display (optional)

### Overview

In this tutorial, you will download two pretrained image classifiers from the gallery, compile the classifiers for the Raspberry Pi, and write a Python script that invokes the classifiers in a round-robin fashion and displays results side by side. When the Python script runs on the Pi, you will be able to point the camera at a variety of objects and compare both result and evaluation time per frame of the the classifiers.

For the example in this tutorial, we will download a real-valued model and a binarized version of that model to compare side by side. You'll see that the binarized model is much smaller, but less accurate. Runtime characteristics of the models differ too: different models have different resource and power requirements. Play around with other models in the gallery after this tutorial to find the one that best suits your particular scenario.

### Prerequisites
We recommend that you are familiar with the concepts in [Getting Started with Image Classification on the Raspberry Pi](/ELL/tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/).

1. We will use `Python 3.6` for this tutorial on your dev box.
We highly recommend using the miniconda or full anaconda python environment because it comes with many
handy tools like `curl` which we will use later on.

2. You will also need a simple web cam or a pi cam.  If you don't have one handy, we will show you how to load
static images or .mp4 videos and process those instead.

3. Additional Software is needed on your Raspberry Pi - See [Setup Raspberry Pi](/ELL/tutorials/Setting-Up-your-Raspberry-Pi). 

4. You will need to be sure you built ELL as per the ELL INSTALL-xxx.md instructions at the root of this git repo.  You will need to build ELL after you install Python from your activate your conda environment, so that the `CMake` step picks up on the fact that you have Python 3.6 installed.

### Download pre-trained models
Make a new directory named `sideBySide` in the `build/tutorials` folder which is where we will download a pre-trained model. 
```
mkdir sideBySide
cd sideBySide
```
ELL has a [gallery of pre-trained models](/ELL/gallery). For this tutorial, we'll use models trained on the ILSVRC2012 data set. Along with the models, you'll want to download a labels file that has friendly text names for each of the 1000 classes that the models are trained to recognize.
We'll use the (ILSVRC2012_labels.txt)(https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/ILSVRC2012_labels.txt) labels file and the following two model files:
* A real-valued model at https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3CMCMCMCMCMCMC1A/d_I224x224x3CMCMCMCMCMCMC1A.ell.zip
* A binarized version at https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3NCMNBMNBMNBMNBMNBMNC1A/d_I224x224x3NCMNBMNBMNBMNBMNBMNC1A.ell.zip

Note that the model files are zipped, and have long named indicating their architecture. For convenience, we'll just want to save it locally as `ell1.zip` and `ell2.zip`:
```
curl --location -o labels.txt https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/ILSVRC2012_labels.txt
curl --location -o ell1.zip https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3CMCMCMCMCMCMC1A/d_I224x224x3CMCMCMCMCMCMC1A.ell.zip
curl --location -o ell2.zip https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3NCMNBMNBMNBMNBMNBMNC1A/d_I224x224x3NCMNBMNBMNBMNBMNBMNC1A.ell.zip
```
Inside `ell1.zip` is the ell model named `d_I224x224x3CMCMCMCMCMCMC1A.ell`, so unzip the archive to the current directory (`sideBySide`). 
Inside `ell2.zip` is the ell model named `d_I224x224x3NCMNBMNBMNBMNBMNBMNC1A.ell`, so unzip the archive to the current directory (`sideBySide`). 

Recent versions of git come with the `unzip` tool:
```
unzip ell1.zip
unzip ell2.zip
```
 Rename the `d_I224x224x3CMCMCMCMCMCMC1A.ell` model file to `model1.ell` and rename the `d_I224x224x3NCMNBMNBMNBMNBMNBMNC1A.ell` model file to `model2.ell`:

| Unix    | `mv d_I224x224x3CMCMCMCMCMCMC1A.ell model1.ell` <br> `mv d_I224x224x3NCMNBMNBMNBMNBMNBMNC1A.ell model2.ell` |
| Windows | `ren d_I224x224x3CMCMCMCMCMCMC1A.ell model1.ell` <br> `ren d_I224x224x3NCMNBMNBMNBMNBMNBMNC1A.ell model2.ell` |

You should now have a `labels.txt` file, a `model1.ell` file and a `model2.ell` file in the `sideBySide` folder.
### Wrap the models in Python callable modules
For this tutorial we want to call the model from Python.  ELL provides a compiler that takes a model and compiles it into code that will run on a target platform - in this case the Raspberry Pi running Linux, so it generates code for armv7-linux-gnueabihf, and for the cortex-a53 CPU.
Similar to the [Getting Started with Image Classification on the Raspberry Pi](/ELL/tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/) tutorial, we'll use the `wrap.py` utility, this time with the `--oudir` option to put the models into different directories:

````
python ../../tools/wrap/wrap.py labels.txt model1.ell -lang python -target pi3 -outdir model1
python ../../tools/wrap/wrap.py labels.txt model2.ell -lang python -target pi3 -outdir model2
````
You should see output similar to the following:

````
compiling model...
generating python interfaces for model1 in model1
running llc...
success, now you can build the 'model1' folder
...
compiling model...
generating python interfaces for model2 in model2
running llc...
success, now you can build the 'model2' folder
````

We also want to copy some additional python code to your Raspberry Pi for the purpose of running this tutorial. You can also copy a static image over for testing:

| Unix    | `cp ../../tools/utilities/pythonlibs/*.py .`       <br> `cp ../../tools/utilities/pitest/coffeemug.jpg .`   |
| Windows | `copy ..\..\tools\utilities\pythonlibs\*.py .` <br> `copy ..\..\tools\utilities\pitest\coffeemug.jpg .` |

You should now have a `sideBySide` folder containing `model1` and `model2` directories as well as some helpful python utilities which we'll use in the next section.

## Call your model from a Python app
Create a new text file called `sideBySideDemo.py` in your `sideBySide` folder. We'll add Python code to:
* Parse the list of models
* Load the compiled models
* Get an image
* Run the image through each model in turn
* Compose an image made up of the results from each model's predictions
* Show the tiled image result

If you don't want to type it out, the script can be found [here](/ELL/tutorials/Comparing-Image-Classification-models-side-by-side-on-the-Raspberry-Pi/sideBySideDemo.py), otherwise follow along below.

First, we need to import the libraries we'll be using in this app, which include system ultilities, numpy and demoHelper that we copied over from ELL utilities:
```python
import sys
import os
import numpy as np
import cv2
import demoHelper as d
```
Next, we're going to define a helper function that will split the argument string specifying the models to run side by side, and instantiate a model helper wrapper object for each, returning an array of all the wrapped models. We used the model helper class before in [Getting Started with Image Classification on the Raspberry Pi](/ELL/tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/), it provides handy functions to load and call ELL models.

```python
def get_model_helpers(demoArgs):
    """
    Returns a list of model helpers, initialized from the commandline arguments in demoArgs
    """
    models = []
    numModels = 0

    # Split the labels string on commas.
    demoArgs.labelsList = [x.strip() for x in demoArgs.labels.split(',')]

    if demoArgs.models:
        # Split the models string on commas
        demoArgs.modelsList = [x.strip() for x in demoArgs.models.split(',')]
        numModels = len(demoArgs.modelsList)
        demoArgs.compiledList = [None] * numModels
    else:
        # Split the compiled string on commas
        demoArgs.compiledList = [x.strip() for x in demoArgs.compiledModels.split(',')]
        numModels = len(demoArgs.compiledList)
        demoArgs.modelsList = [None] * numModels
    # If the number of elements in the labelsList is 1, then use the same labels file
    # for all models
    if (len(demoArgs.labelsList) == 1):
        demoArgs.labelsList = demoArgs.labelsList * numModels

    # Iterate over the model list and instantiate a helper for each.
    # Interactions with each model will be done via this wrapper class.
    helperArgs = demoArgs
    for i in range(numModels):
        # Instantiate a new helper for the model and initialize it
        helper = d.DemoHelper()
        helperArgs.labels = demoArgs.labelsList[i]
        helperArgs.model = demoArgs.modelsList[i]
        helperArgs.compiledModel = demoArgs.compiledList[i]        
        helper.initialize(helperArgs)
        helper.init_image_source()
        # Add it to the list
        models.append(helper)

    return models
```
Next, we'll define the main function, which contains the primary application logic for this tutorial. We need to parse the commandline arguments, and call our helper function we just defined to get a list of models we will use. 
```python
def main(args):
    """Main function for the Side By Side tutorial"""
    demoArgs = d.get_common_commandline_args(args, 
        "Runs a number of ELL models that predict the same categories, passing images from camera or static image file\n"
        "in a round-robin fashion. The output is a tiled image, where each tile is the result of one model."
        "Either the ELL model files, or the compiled models' Python modules must be given,\n"
        "using the --models or --compiledModels options respectively.\n"
        "Example:\n"
        "   python sideBySideDemo.py categories1.txt,categories2.txt --compiledModels models/pi3/model1,models/pi3/model2\n"
        "   python sideBySideDemo.py sameCategories.txt --models model3.ell,model4.ell\n"
        "This shows opencv window with image classified by the models using given labels")
    models = get_model_helpers(demoArgs)
    if (len(models) < 1):
        print('Found no models to run')
        sys.exit()

```
We'll use a helper class called TiledImage to crete the output for this tutorial. The TiledImage class composes a set of images into one which can be displayed by OpenCV. Each tile in the output image is the result of passing a frame to a model instance.
```python
    tiledImage = d.TiledImage(len(models))
```
Declare a loop so we can keep grabbing frames to push through the models. For simplicity, just ask the first model to grab a frame. The same frame will be passed to each model.
```python
    done = False
    while (not done):
        # Grab next frame
        frame = models[0].get_next_frame()
```
We want to pass the frame to each model in turn. For fairness, we'll randomize the order so that no one model benefits from cache locality more than another.
```python
        # Run through models in random order to get a fairer average of evaluation time
        modelIndexes = np.arange(len(models))
        np.random.shuffle(modelIndexes)
```
Iterate over the models. `frame` now holds image data for the model. However, it often cannot be used as-is, because models are typically trained with:
* specific image sizes e.g. 224 x 224 x 3
* specific ordering of color channels e.g. RGB. Our helper uses OpenCV to grab images from the image source (file or webcam). Their size is dependent on the source, and the ordering is always BGR from OpenCV. Therefore, we need to crop and or resize the image while maintaining the same aspect ratio, and reorder the color channels from BGR to RGB. Since this is such a common operation, the helper implements this in a method called prepare_image_for_predictor:
```python
        for modelIndex in modelIndexes:
            model = models[modelIndex]
            # Prepare the image to send to the model.
            # This involves scaling to the required input dimension and re-ordering from BGR to RGB
            data = model.prepare_image_for_predictor(frame)
```

We are now ready to get a classify the image in the frame. The model has a 'predict' method, which will return a list of probabilities for each of the 1000 classes it can detect:
```python
            # Get the compiled model to classify the image, by returning a list of probabilities for the classes it can detect
            model.predict(data)
```
Note that this is just an array of values, where each element is a probability between 0 and 1. It is typical to reject any that do not meet a particular threshold, since that represents low confidence results. Re-ordering so that we get only the Top 5 predictions is also useful. The index of the prediction represents the class, the value represents the score. We can use the labels file to match the index of the prediction to its text label, and then construct a string with the label and score.
We'll use this string as header text for each tile.
```python
            # Get the (at most) top 5 predictions that meet our threshold.
            top5 = model.get_top_n(model.results, 5)

            # Turn the top5 into a text string to display
            header_text = "".join([model.get_label(element[0]) + "(" + str(int(100 * element[1])) + "%)  " for element in top5])
            # Draw the prediction text as a header
            modelFrame = np.copy(frame)
            model.draw_header(modelFrame, header_text)
```
Each model wrapper is keeping track of how long the `predict` function took to return a result. This is the model's evaluation time, and we will set it as part of the footer text:
```python
            # Draw the evaluation time as the footer
            evaluationTime = model.get_times() * 1000 # time in ms
            if (evaluationTime is not None):
                footerText = '{:.0f}'.format(evaluationTime) + 'ms/frame, ' + model.model_name
                model.draw_footer(modelFrame, footerText)
```
Lastly, update the tiled image with this frame. Each output frame composed from results of the model will be one tile in the output:
```python
            # Set the image with the header and footer text as one of the tiles
            tiledImage.set_image_at(modelIndex, modelFrame)
            done = tiledImage.show()
            if done:
                break

if __name__ == "__main__":
    args = sys.argv
    args.pop(0) # when an args list is passed to parse_args, the first argument (program name) needs to be dropped
    main(sys.argv)
```
Your `sideBySide` folder is ready to copy to your Raspberry Pi. You can do that using the ‘scp’ tool. On Windows you can use WinSCP.

### SSH into Raspberry Pi

Now log into your Raspberry Pi, either remotely using SSH or directly if you have keyboard and screen attached.

Find the `model1` folder you just copied over using scp or winscp and run the following:

````
cd model1
mkdir build
cd build
cmake ..
make
cd ..
cd ..
````
This builds the Python Module that is then loadable by the demo Python scripts. Do the same for the `model2` folder:

````
cd model2
mkdir build
cd build
cmake ..
make
cd ..
cd ..
````

### Process a static image 
Now if you followed the [Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-Up-your-Raspberry-Pi) you should have a miniconda
environment named py34.  So to run the tutorial do this:

````
source activate py34
python sideBySideDemo.py labels.txt --compiledModels model1/model1,model2/model2 --image coffeemug.jpg
````

If you have a display connected you should see the screen shot at the top of this page.

### Process Video
If you have a USB camera attached to your Pi then you can also use ELL to process video frames:

````
python sideBySideDemo.py labels.txt  --compiledModels model1/model1,model2/model2
````

You will see the same kind of window appear only this time it is showing the video stream.
Then when your camera is pointed at an object that the model recognizes you will see the label and 
confidence % at the top together with an estimated frame rate.

`Tip`: for quick image recognition results you can point the video camera at a web image of a dog 
on your PC screen.  ImageNet models can usually do a good job recognizing  different dog breeds and 
many types of African animals.

## Next steps
Different models have different characteristics. For example, some are slow but accurate, while others are faster and less accurate. Some have different power draw than others.

Experiment with which model works best for you by downloading other models in the [ELL gallery](/ELL/gallery/).

Try these related tutorials:
* [Fun with Dogs and Cats](/ELL/tutorials/Fun-with-Dogs-and-Cats/)
* [Comparing Image Classification models side by side on the Raspberry Pi](/ELL/tutorials/Comparing-Image-Classification-models-side-by-side-on-the-Raspberry-Pi/)

### Toubleshooting

If you run into trouble there's some troubleshooting instructions at the bottom of the 
[Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-Up-your-Raspberry-Pi).