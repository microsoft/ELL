---
layout: default
title: Using ProtoNN
permalink: /tutorials/Introducing-ProtoNN/
---

# Using ProtoNN

*by Chris Lovett*

ELL includes a new model trainer called ProtoNN.  ProtoNN created compressed yet accurate kNN models for resource scarce devices.
A typical ProtoNN model can be 10 times smaller in memory usage than the same CNTK model and yet produces similar accuracy.

To learn how to use ProtoNN you will use the MNIST dataset, which is a classic in machine learning.  MNIST models learn how to recognize
hand written digits (0-9), and the Microsoft Cognitive Toolkit (CNTK) provides sample code and a way to download the training data.
In this tutorial you will first train the CNTK model, then you will do the same using ProtoNN and then you can compare the results.

To read more about the research behind ProtoNN see [gupta17.pdf](http://manikvarma.org/pubs/gupta17.pdf).

#### Materials

* Laptop or desktop computer
* Raspberry Pi (optional)

#### Prerequisites

* Install ELL on your computer ([Windows](https://github.com/Microsoft/ELL/blob/master/INSTALL-Windows.md), [Ubuntu Linux](https://github.com/Microsoft/ELL/blob/master/INSTALL-Ubuntu.md), [macOS](https://github.com/Microsoft/ELL/blob/master/INSTALL-Mac.md)).
* To import a model from the CNTK file format, you must install the [CNTK python packages](https://docs.microsoft.com/en-us/cognitive-toolkit/setup-cntk-on-your-machine).

## Step 1

The first step in this tutorial is to download MNIST and the CNTK sample code.

Download the [SimpleMNIST.py](https://github.com/Microsoft/CNTK/blob/release/2.2/Examples/Image/Classification/MLP/Python/SimpleMNIST.py) script.

Change line 102 before it loads the test data, to save the CNTK model to a file, since we need this for the ELL importer.

```python
    trainer.model.save("mnist.cntk")
```

Download the [install_mnist.py and mnist_utils.py](https://github.com/Microsoft/CNTK/tree/master/Examples/Image/DataSets/MNIST) scripts
from CNTK git repo and run them from a new temporary folder.  This produces two text files in your new folder so you should see:

```
Test-28x28_cntk_text.txt
Train-28x28_cntk_text.txt
```

And as you may have guessed the "Train" file is for training the model and the "Test" file is for testing the trained model.

Now run your modified SimpleMNIST.py script to train the CNTK model:

```shell
python SimpleMNIST.py
```
 
This shouldn't take too long, on a Xeon Intel Core-i7 desktop it takes about 1.5 minutes.
CNTK trains a very accurate model.  The error reported by CNTK is 0.023600, which means it has an accuracy of
97.6%.  You can see the CNTK neural network is very simple, it just contains two Dense layers.

## Step 2

Now you can import this model to get the equivalent ELL model so we can compare this model with the ProtoNN model that we build in Step 3.

Run this command to import the CNTK model into ELL:

```shell
python cntk_import.py mnist.cntk
```

Note that `cntk_import.py` lives under the `tools/importers/CNTK` folder of
your ELL git repo.

After running this you should see an `mnist.ell` file which is about 1.8 megabytes in size.

You can compile this model using the `wrap.py` tool as shown in the 
[Getting started tutorials](/ELL/tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/) if you want to test it out.

## Step 3

Ok, now you are ready to train the ProtoNN model.  First you will need to convert the test data into an ELL dataset because this is what the ProtoNN traininer expects to get.

Run the following commands to convert to the ELL dataset format.
[convert_dataset.py](convert_dataset.py) is included with this tutorial.

```shell
python convert_dataset.py Train-28x28_cntk_text.txt
python convert_dataset.py Test-28x28_cntk_text.txt
```

This will take a while, so please be patient.  Now run the ProtoNN trainer. This tool is built by the ELL build, and you will find it in the build/bin folder (or build/bin/Release on Windows).

```
protoNNTrainer.exe -v --inputDataFilename Train-28x28_cntk_text.ds -dd 784 -sw 1 -sb 0.3 -sz 0.8 -pd 10 -l 10 -mp 20 --outputModelFilename mnist.ell --evaluationFrequency 1 -plf L4
```

You can also train the model from Python.  See [TrainMNIST.py](TrainMNIST.py) included in this tutorial.

You will see output like this:

```
Loading data ...
Training ...
Iteration 0
Iteration 0
Iteration 0
Iteration 1
```

It will perform 20 iterations, then it will test the model accuracy, report the result, then it will save the new `mnist.ell` file. 

```
Accuracy: 0.937067
```

The accuracy is less than CNTK but if you look at the model size it is
111 kb, compared to 1.8 mb, so the model is about 16 times smaller.
Not bad for a 3.9% drop in accuracy.

If you print the `mnist.ell` model you will see a simple graph with 
an input node, a ProtoNN node and an output node because
the ProtoNN algorithm is implemented by a single ELL node type.

To test the ELL "reference implementation" of this model (the reference
implementation is an "uncompiled" version of the algorithm that runs
more slowly, but allows a quick test to make sure it is working before
we go to the effort of wrapping it in a compilable project).

Start a new python file named [test_reference_model.py](test_reference_model.py)
and import the required modules:

```python
import sys
import numpy as np
import time
```

Next, import ELL, but in order to do that you need to tell Python
where to find it, which can be done using `sys.path.append` as follows:

```python
sys.path.append("d:/git/ell/ell/build/interfaces/python")
sys.path.append("d:/git/ell/ell/build/interfaces/python/release")
import ELL
```

The above paths will vary depending on your operating system and the location of your ELL git repo.

Ok, now you can load the ProtoNN model produced by the above trainer using ELL as follows:

```python
mnist = ELL.ELL_Map("mnist.ell")
```
and you can check the expected input size as follows:

```python
input_shape = mnist.GetInputShape()
input_size = input_shape.rows * input_shape.columns * input_shape.channels
print("input size=%d" % (input_size))
```

You should see it print "784" because MNIST uses 28x28 sized images.

Next, you can load the Test dataset with the following:

```python
dataset = ELL.AutoSupervisedDataset()
dataset.Load("Test-28x28_cntk_text.ds")
```

A "supervised" dataset is a dataset that includes the right answers for each
training example.  This allows supervised training algorithms to check their
training progress by comparing whether they are producing against the expected
right answers.  An "AutoSupervisedDataset" figures out the best storage model
to use (either sparse, dense, or binary storage) depending on how many zeros
are found in the data, and on whether the data is binary or not.

Lastly, run the test and print the accuracy:

```python
accuracy = run_test(mnist, dataset)
print("Accuracy %f" % (accuracy))
```

`run_test` enumerates the test dataset calling ComputeDouble for each example.
The dataset is labelled so it compares the result from ELL against the
expected result and tallies up the correct answers.

```python
def run_test(mnist, dataset):
    total = dataset.NumExamples()
    true_positive = 0
    for i in range(total):
        example = dataset.GetExample(i)
        result = mnist.ComputeDouble(example.GetData())
        answer = ELL.DoubleArgmax(result)
        expected = example.GetLabel()
        if answer == expected:
            true_positive += 1
    accuracy = true_positive / total
    return accuracy
```

That's it!  You should see the final test accuracy printed which is:

```
Accuracy 0.930700
```

This is slightly lower than the "training" accuracy, which is expected since the model never saw the test set before, so this is a more true representation of the accuracy of the model.

## Step 4

You can now compile the `mnist.ell` model using the steps outlined in 
[Getting started tutorials](/ELL/tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/).
Namely, use the `wrap.py` tool to create a compiled project for your host PC then build it.

Once you have done that you should have a `host` folder containing the compiled `MNIST` model.
You can test this model with the `Test-28x28_cntk_text.ds` validation set by running the following:

```
python test_compiled_model.py Test-28x28_cntk_text.ds
```

You should see the exact same accuracy as before, namely, 0.930700.  This means the compiled code
is working just as well as the ELL reference implementation.

You can now also use `wrap.py` to generate binaries that will run on a different target platform
like the Raspberry Pi and then have MNIST running there also:

```
python d:\git\ell\ell\tools\wrap\wrap.py mnist.ell -target pi3
copy Test-28x28_cntk_text.ds pi3
copy test_compiled_model.py pi3
```

Copy the pi3 folder to your Raspberry Pi, build it then run the same test_compiled_model.py command line.
Note: you will need to clone and build ELL on the Raspberry Pi to get the same ELL.AutoSupervisedDataset
class that is used by this test.


## Step 5

For completeness, you can also train the ELL model from Python, and an example of
that is included in this tutorial.  See [train_protonn.py](train_protonn.py).
This should give you the same ELL model that the `protoNNTrainer` command created.
