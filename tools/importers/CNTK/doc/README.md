# CNTK Importer

## Overview of the importer

In CNTK, models are broken down into a tree of functions. With CNTK 2.0, functions
can be grouped into "blocks". This is handy, since layers are often groups of
functions e.g. a `Convolutional` block can be made up of convolution, bias and
activation.

The CNTK importer uses the [CNTK Python API](https://cntk.ai/pythondocs/) to
convert CNTK layers to equivalent ELL layers. 

## Importing a model

Importing a model means converting a CNTK model into an ELL predictor.
To do this, provide the CNTK model file to the top-level
`predictor_from_cntk_model` function:
```
from cntk_to_ell import predictor_from_cntk_model
ell_predictor = predictor_from_cntk_model('myCntkModel.dnn')

```
The CNTK importer library works on models saved with CNTK 2.0 and later. Models
from version 1.x are not supported. These models should be loaded and re-saved
in CNTK 2.0 or later.

## Visualizing a model

CNTK has a `plot` function in the `cntk.logging.graph` module, which can produce
a visual representation of a model. This is incredibly helpful in visualizing
what the layers of the model are, and what the values of the dimensions for the
input/output/parameters/weights are for each block/layer.

### Pre-requisites

* Install CNTK for Python, following the instructions [here]( https://docs.microsoft.com/en-us/cognitive-toolkit/Setup-CNTK-on-your-machine)

* Install pydot_ng and GraphViz for Python
```
pip install graphviz 
pip install pydot_ng 
```

On Windows, additional steps may be required if Python is not able to find Graphviz:
* Download and unzip [Graphviz](http://www.graphviz.org/)
* Update the PATH environment variable to point to the location of graphviz's
bin folder.  For example: `set PATH=%PATH%;path_to_graphviz\bin`

### Plotting the model

Here is a script that can plot the model to png file. Save this as plot_model.py: 

```
# Python 3
import sys, getopt 
from cntk import load_model 
import cntk.logging.graph as graph 
 
def plot_model(modelFile, outputFile): 
    z = load_model(modelFile) 
    text = graph.plot(z, outputFile) 
    print(text) 
    return 
 
def main(argv): 
    modelFile = '' 
    outputFile = 'out.svg' 
    try: 
        opts, args = getopt.getopt(argv,"hm:o:",["modelFile=","outputFile="]) 
    except getopt.GetoptError: 
        print('plot_model.py -m <modelFile> -o <outputFile>') 
        sys.exit(2) 
    for opt, arg in opts: 
        if opt == '-h': 
            print('plot_model.py -m <modelFile> -o <outputFile>') 
            sys.exit() 
        elif opt in ("-m", "--modelFile"): 
            modelFile = arg 
        elif opt in ("-o", "--ofile"): 
            outputFile = arg 
    if (len(modelFile) == 0): 
        print('plot_model.py -m <modelFile> -o <outputFile>') 
        sys.exit(2) 
         
    print('Plotting model ', modelFile, ' to ', outputFile) 
    plot_model(modelFile, outputFile) 
 
if __name__ == "__main__": 
   main(sys.argv[1:])
```

For example, to plot the VGG16 model from ELL's getting started tutorial: 
 
```
python plot_model.py -m VGG16_ImageNet_Caffe.model -o vgg16.svg
```

This produces a PNG file that shows a graph of the functions/blocks in a model.
For example, here's a snippet of the plot for the VGG16 model:

![vgg16 snippet](vgg16-snippet.png)

This snippet shows: 
* A `Convolution` block whose input is 14 x 14 x 512. Notice how CNTK lists it
as 512 x 14 x 14. For this function, the order is filters x rows x columns,
whereas ELL is rows x columns x filters. This is one of main functions of the
importer: change the order of the weights so that they are compatible with ELL).
You can see that `Convolution` has weights (conv5_3.W) and bias (conv5_3.b).
It also applies a `ReLU` activation on the output. 
* The `Convolution` block feeds into a `Pooling` function. The plot does not
show the type of pooling, so you will need to inspect the attributes of the
pooling function to determine whether this is `Max` or `Average` pooling (the
importer code has an example of this). The output of the `Pooling` layer
reduces the dimensions to 7 x 7 x 512.
* The output of the `Pooling` layer goes to a `linear` layer, which is CNTK's
dense or fully connected layer. Notice the weights and bias, followed by `ReLU1`
activation. The output of this is a vector of size 4096. In ELL, we represent
this as a Tensor of 1 x 1 x 4096. The importer code has examples of processing
`linear` layers that take in a 3D tensor and output a vector. 
 
## Updating the importer to support more CNTK layers

The CNTK importer is written to handle evaluating convolutional networks for
computer vision.  Therefore, it doesn't cover all possible CNTK layers, blocks,
or functions. Layers in a model that are not supported by the importer are
skipped over.

If a skipped CNTK layer is actually required by your model to correctly evaluate,
the importer can be updated to handle it, assuming that there is an equivalent
ELL layer that exists. The list of ELL neural network layers can be found [here](../../../../libraries/predictors/neural/include).

[Updating.md](Updating.md) describes the steps to update the importer to handle
more CNTK layers.


