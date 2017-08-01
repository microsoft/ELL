# Updating the CNTK Importer

This section outlines a general approach for updating the [CNTK importer](README.md). 

You typically would update the importer if all 3 conditions apply:
1. Your model contains layers that are needed for evaluation
2. These layers are being skipped by the importer as irrelevant
3. There are existing [ELL layers](../../../../libraries/predictors/neural/include)
that can support the CNTK functionality, so the importer can simply be updated to
convert the new CNTK layers to those ELL layers.

## Step1: Add a unit test 
Since CNTK has a Python Api, the recommended way to add a new layer to the importer
is to first add a unit test for that layer.   
Edit [cntk_to_ell_importer_test.py](../test/cntk_to_ell_importer_test.py).
Look at an existing test, say test_convolution_layer.

The general approach is to: 
* Create the CNTK layer using the [CNTK Python API](https://cntk.ai/pythondocs/)
* Use numpy to create the weights/bias/parameters you need. Either fill them with
random values, or use `numpy.arange` to create a known set of non-zero entries:
```
weightValues = np.arange(90, dtype=np.float_).reshape(5, 2, 3, 3)
```
* Read the CNTK documentation for that layer to determine the order of those weights.
For example, in convolution, the weights order is specified as: 
```
filters, channels, rows, columns
```
* Create the corresponding ELL layer. Set the weights to be the same as those from CNTK. 
Note: Since CNTK's order is different from ELL's, first re-order the weights. The
importer has [converter functions](../lib/cntk_converters.py) to re-order numpy arrays
and turn them into ELL.Tensors. 
  * For an example, see `get_float_tensor_from_cntk_convolutional_weight_parameter(. . .)` 
  * If an existing function doesn't work for you, add a new one to the importer. Your unit 
  test should call this function to re-order the weights. That way, the re-ordering is
  shared between the unit test and the importer and ensures the re-ordering is properly
  validated.

## Step 2: Add processing of the CNTK layer to the importer

Once you have the unit test passing, you can be confident that the ordering is correct.
The next step is to add the processing of that layer to the CNTK importer.

File structure of the importer:

* `cntk_to_ell.py`: entry point
* `lib/cntk_layers.py`: layer processing classes for CNTK to ELL. This is the place to
start your updates.
* `lib/cntk_converters.py`: converters from CNTK or related data structures to ELL's 
* `lib/cntk_utilities.py`: miscellaneous utility functions that don't fall in layers or
converters 
* `custom_functions.py`: user functions for `BinaryConvolution` that are used for training,
evaluation, and importing.

There are a couple of things to update for the importer: 
* Add a new Layer class 
  * In [cntk_layers.py](../lib/cntk_layers.py), define a class that extends `BaseLayer`
    * Implement the `__init__` method in your layer to specify the conditions where the
    layer will be created, and to call `BaseLayer`'s `__init__` method.
    * If the conditions for creating your layer are not met, raise an exception here so
    that that layer gets skipped.
  * Implement the `process` method with logic to convert the CNTK layer to ELL.  
  * Padding: if the layer has special padding requirements, there are two options: 
    * If the padding requirements can be expressed in terms of `ELL.PaddingScheme`,
    initialize the padding scheme in your `__init__` method before calling `BaseLayer`'s
    `__init__` method.  For example, the padding scheme on a `MaxPooling` layer must be 
    `PaddingScheme.min`, whereas on an `AveragePooling` layer, it must be
    `PaddingScheme.zeros`.
    * If there is extra processing logic you need to add beyond setting the padding scheme, 
    override the `get_input_padding_parameters` method. See `ConvolutionLayer` for
    an example of how to do this.

* Update `LayerFactory.get_layer_object` method to map a CNTK layer to the new Layer class
  * This function looks at each CNTK operation or functional block and based on the
  operation name, creates a class to produce the corresponding ELL layer. In CNTK, the
  operation can either be a function or a `block` function. 
  Block functions are really groups of functions (e.g. `Convolution` could be just the
  `convolution` function, or could be a `Convolution block` which has
  `convolution` + `bias` + `activation` functions in the block. 
  * Whether an operation is a block or not is determined by how the model was constructed.
  You may have to account for both types of models, i.e. models that are using blocks and
  models that aren't. 
  * Update this function to process your new layer/operation when by calling the
  constructor of your layer class. 

## Step 3: Test your new Layer class

Besides running the importer on your model, you can also compare the output of your ELL
layer with a truncated version of its corresponding CNTK model.   
* See `compare_predictor_output` in [cntk_to_ell_importer_test.py](../test/cntk_to_ell_importer_test.py),
and its usage in `CntkBinarizedModelTestCase`
* To target a specific layer, set the `maxLayers` parameter to the `index+1` of that
layer (where `index` is where your layer lives in the filtered CNTK layers list - the
output of `cntk_layers.get_filtered_layers_list`).
  * For example, if the `BinaryConvolution` layer is the 4th CNTK layer (index = 3)
  in the filtered layers list, set `maxLayers` to 4.    
* To test the full model, set `maxLayers` to None. This will compare the ELL model
with the original (non-truncated) CNTK model.  Note that you may have to post-process
the result slightly differently if the full model performs training-related
post-processingm while the ELL model doesn't.  For example, `compare_predictor_output`
compares the top-N results instead of comparing the raw model output.