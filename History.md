## 3.0.3
- Fix VS 2019 build.

## 3.0.2
- Add FastGRNN node to ELL, train_classifier.py and the onnx_importer.

## 3.0.1
- Fix make_vad.py so the VAD model works properly.
- Fix smoothing of classifier output.
- Improvements to noise mixing in audio training scripts
- Use featurizer metadata to get correct defaults for some audio script parameters
- Improvements and code clean up in view_audio.py
- make_dataset will fill the featurizer internal buffer before returning features so it doesn't featurize zeros.
- onnx_importer: fixes to ConstantFill, Reshape, Slice, Transpose, and support for constant folding involving Cast, and Mul and fix handling of nodes with multiple outputs.
- Fix bug causing OptimizeReorderDataNodesTransformation to fail caused by input port and memory layout mismatch
- move to SWIG 4.0.0.

## 3.0.0
- Improve Python API so Map and CompiledMap have a way of calling compute on models with multiple inputs and outputs (see ComputeList)
- Improve Python Callback handling so Python apps can handle models with any number of and type of callbacks.
- Simplify datasetFromImages.py so it can create training, testing and validation set from one set of images on disk.
- Update repurposing tutorial setup to take advantage of dataFromImages.py changes
- Update index.md to fix bug in Linux instructions
- Fix FFTNode so that behavior is identical to NumPy behavior
- Update make_featurizer.py to calculate features correctly

## 2.5.4
- Add support for ONNX GlobalAveragePooling
- Publish 13 new audio models trained using Azure ML
- Add ONNX importer support for ConstantOfShape
- Add a -mean option to audio test runs to output mean prediction score
- Fix InputNode unarchival to correctly set the memory layout

## 2.5.3
- Fixed build break in C++ tutorial on Windows

## 2.5.2
- Moved to LLVM 8.0
- Added Asparagus and Pear models to the gallery, removed Elderflower
- Added support for image preprocessing metadata and image normalization defaults for pytorch + imagenet
- Updated find_ell.py (and thus other python tools) to work if your working directory contains ELL's build output

## 2.5.1
- Fixed API wrapper for PortMemoryLayout to produce correct extent
- Fixed processing of ConcatenationNode in importer engine
- Added four build target aliases that build groups of targets: bindings, libraries, tests, and tools
- Add support for ONNX Cast operator (thanks to Ehsan Azar PR #218)
- Fixed build error on macOS (GitHub issue #216)
- Added model per-layer optimizing scripts

## 2.5.0
- Improve audio python library and update audio training tutorial
- Add support for non-unrolled loops in the Value library
- Add new methods to Python API to support FastGRNN models
- Add easy node-creation functions
- Fix bugs in ReinterpretLayoutNode
- Fix importing of DenseNet models
- Improve FFT and MelFilterBank nodes so that non-power of 2 sized input buffers can be processed
- Fix darknet importer
- Add shuffling of test and validation sets to audio train_classifier
- Update OpenBLASSetup.cmake so it can find a locally built version of OpenBLAS
- Fix compiling for cortex-m4 targets
- Add a link back to the original model file name in our emitted header file
- Clean up Python API, removing "TensorShape" specific API in favor of the more general PortMemoryLayout
- Fix bug in Microphone and WavReader classes so they properly handle auto-resampling of audio with multiple channels
- Add an -auto_scale option to audio scripts to control whether audio is scaled or not before featurization
- Add ability to mix noise with audio during make_dataset process
- Add support for shuffling and cleaning the audio training list
- Add many new options to audio train_classifier, including ability to control learning rate schedulers
- Fix link error when building on a Mac
- Fix exception in DetectLowPrecisionConvolutionTransformation when running Compile in Debug mode

## 2.4.0
- Initial release of the finetune utility
- Remove stale references to VS 2015
- Add logging of build tool options
- Improve error reporting of compile failures
- Suppress alias analysis on emitted IR code
- Improve logging in ELL pythonlibs
- Remove volatile attribute from memcpy, memmove, and memset LLVM intrinsic calls
- Allow compilation of models with more than one input or output
- Fix parsing of boolean command-line arguments
- Add a RAM requirement for audio tutorial
- Add support for optimized models
- Fix ONNX import of no-op concat blocks
- Gallery updates for Pi3 specific ELL models
- Added documentation detailing ELL's versioning strategy

## v2.3.7
- Move to LLVM 6.0.1
- Move ELL to C++17
- Add support for python 3.7
- Add support for more ONNX operations
- Added instructions for Raspbian Lite (https://microsoft.github.io/ELL/tutorials/Raspberry-Pi-setup/)
- Add new OpenBLAS include path for Ubuntu 18.04
- Fix reorder data node optimization pass
- Fix debugCompiler to work with gallery models
- Improve reordering of data for simple and Winograd depthwise convolutions
- Fixes for "Getting started with image classification on Raspberry Pi using C++" tutorial
- Fix importing of depthwise separable models
- Fix "Repurposing a pretrained image classifier" tutorial

## v2.3.6
- Initial version of the ONNX importer
- Tools now have a -v/--verbose option that will control the output of additional helpful logging - data
- Demo helper script has been modified to print labels to STDOUT instead of drawing on the image. - This is useful for some training sets like CIFAR
- Couple of new options for wrap.py:
    - --convolution_method: this lets you specify the convolution method (it just passes the value on to the compile executable)
    - --llvm-format gets a new option: obj. This, along with --no-llc-tool and --no-opt-tool let you skip the time-consuming llc and opt steps. To retain compatibility with compile, the object file suffix is now .o.

## v2.3.5
- Fixed typos in some tutorials (thanks @lisaong )
- Added support for emitting relocatable object code
- Added Travis-CI support (thanks @lisaong )
- Updated profiler to work with models from the ELL gallery
- Fix build break for VS2017.7.3 in StlContainerReferenceIterator
- Fix python interface implementation for LSTM layer nodes
- Fix crash in retarget trainer (issue #141)
- Fix docs for opencv setup on raspberry pi
- Improve support for running models on Raspberry Pi Zero
- make_profiler.sh can now be run from anywhere
- Various bug fixes (#161, #157, #149)

## v2.3.4
- Fix importing CNTK models with a Softmax layer
- Add a Reset function to LSTM and GRU models to clear the state

## v2.3.3
- Fixes issue #140
- Helps issue #138 by fixing importing of Darknet models with Fully Connected layers
- Simplified and improved LLVM IR optimization in emitted code
- Fix importer for next round of separable darknet models in CNTK
- Improved Winograd convolution implementation
- Fix building the profiler tool
- Fix running build.sh without any command line args
- Miscellaneous bug fixes

## v2.3.2
- Fix issue #137
- Added resnet models to gallery
- Add support for naive depthwise separable convolution layers

## v2.3.1
- Added more fine grained profiling support for emitted code
- Make find_package for LLVM more resilient to finding the version that ELL needs
- Updated Setting up your Raspberry Pi device for tutorials to move to Raspbian Stretch
- Updated links to YOLOv2 configuration and weights in Getting started with object detection using - region of interest networks
- Added better support for importing models from older versions of CNTK

## v2.3.0
- New tutorials available:
- Getting started with object detection using region of interest networks
- Getting started with audio keyword classification on the Raspberry Pi
- Updated importer to handle the importing of resnet models
- Miscellaneous bug fixes.
