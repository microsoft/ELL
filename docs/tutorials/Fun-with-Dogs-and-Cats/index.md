---
layout: default
title: Fun with Dogs and Cats
permalink: /tutorials/Fun-with-Dogs-and-Cats/
---
# Fun with Dogs and Cats

### Materials

* A Raspberry Pi
* Headphones or speakers for your Raspberry Pi
* A USB webcam

### Overview

This tutorial shows how to group the the predictions returned from an ELL model so you can
respond to any prediction in that group.  To demonstrate this we create a grouping for dogs
and cats from the ImageNet model which contains many different breeds of dogs and cats.

This technique can be useful if you don't care about the detailed breed information.  Even if
your optimized ELL model is not accurate enough for individual breeds, you may still get value out of the model by grouping.

This tutorial then creates a fun response to the recognition of a dog or a cat by playing a sound.  As you probably already guessed, it will bark or meow in response to it seeing a dog or a cat.  This also works in headless mode (no display).
So imagine your pi is watching your lawn, and when a dog arrives, it barks at the dog.
Your kids will have endless fun with this I'm sure.  

### Prerequisites

This tutorial has the same prerequisites as [tutorial 1](/ELL/tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/).

### Download pre-trained model

Be sure to download the same pre-trained ImageNet model used in [tutorial 1](/ELL/tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/).

### Wrap the model in a Python callable module

Do the same steps here as shown in [tutorial 1](/ELL/tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/).

Now you should now have a pi3 folder that is ready to copy to your Raspberry Pi.  

Add the following files to the pi3 folder:
- [doglabels.txt](/ELL/tutorials/Fun-with-Dogs-and-Cats/doglabels.txt)
- [catlabels.txt](/ELL/tutorials/Fun-with-Dogs-and-Cats/catlabels.txt)
- [woof.wav](/ELL/tutorials/Fun-with-Dogs-and-Cats/woof.wav)
- [meow.wav](/ELL/tutorials/Fun-with-Dogs-and-Cats/meow.wav)
- [pets.py](/ELL/tutorials/Fun-with-Dogs-and-Cats/pets.py)

    cp ~/git/ELL/tutorials/Fun-with-Dogs-and-Cats/* pi3

You can now copy this folder using the 'scp' tool.  From Windows you can use [WinSCP](https://winscp.net/eng/index.php).

### SSH into Raspberry Pi

Now log into your Raspberry Pi, either remotely using SSH or directly if you have keyboard and screen attached.

Find the 'pi3' folder you just copied over using scp or winscp and run the following:

````
cd pi3
mkdir build && cd build
cmake ..
make
cd ..
````

This builds the Python Module that is then loadable by the pets.py script.

### Process a static image 

Now if you followed the [Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-Up-your-Raspberry-Pi) you should have a miniconda
environment named py34, so to run the tutorial do this:

````
source activate py34
python pets.py categories.txt --compiledModel model1 --image cat.png
````
And it will classify the image, you should see output like this and you should hear the meow sound:
````
Cat(90%)
````

### Process Video

If you have a USB camera attached to your Pi then you can also use ELL to process video frames:

````
python pets.py categories.txt --compiledModel model1
````

You will see the same kind of window appear only this time it is showing the video stream.
Then when your camera is pointed at an object that the model recognizes you will see the label and 
confidence % at the top together with an estimated frame rate.

When a dog or a cat appears you will also hear the woof.wav file or the meow.wav file play.

`Tip`: for quick image recognition results you can point the video camera at a web image of a dog 
on your PC screen.  ImageNet models can usually do a good job recognizing  different dog breeds and 
many types of African animals.

### How grouping works

Grouping is a very simple concept. The `doglabels.txt` and `catlabels.txt` files are simply
subsets of the larger file containing 1000 classes of objects.  The `pets.py` script then simply
takes the prediction from the ELL model and checks to see if the result is in one of these lists:
````
top5 = self.helper.get_top_n(predictions, 5)
text = ""
if (len(top5) > 0):
    winner = top5[0]
    label = winner[0]
    if (label in self.dogs):
        text = "Dog"
    elif (label in self.cats):
        text = "Cat"

````
So now we know if text comes back with "Dog" then one of the many dog breeds recognized by the
ImageNet model has been returned in the top confidence spot, namely, top5[0].

To play sounds we use the built-in Raspberry Pi "aplay" command.  You can also run this on
your Windows PC and it will use the windows `winsound` library which is built into the windows
Python runtime.

### Toubleshooting

If you run into trouble there's some troubleshooting instructions at the bottom of the 
[Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-Up-your-Raspberry-Pi).


### Licenses

The dog barking sound was recorded by davidmenke and published by freesounds.org under
[Creative Commons License Zero](https://creativecommons.org/publicdomain/zero/1.0/).

The cat meow was recorded by blimp66 and published by freesounds.org under
[Creative Commons License Attribution](https://creativecommons.org/licenses/by/3.0/)
then the sound was edited, downsampled and converted to wav format to shrink the file size.