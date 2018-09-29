# Note:

This ONNX importer is still a work in progress. Currently, the only supported ONNX models are those exported from PyTorch.

### Environment setup

You should have both [ONNX]("https://onnx.ai/") and [ELL]("https://microsoft.github.io/ELL") installed. To install ELL follow the instruction for your specific os ([Windows]("https://github.com/Microsoft/ELL/blob/master/INSTALL-Windows.md"), [Ubuntu Linux]("https://github.com/Microsoft/ELL/blob/master/INSTALL-Ubuntu.md"), [macOS]("https://github.com/Microsoft/ELL/blob/master/INSTALL-Mac.md") ). To install [ONNX]("https://onnx.ai/") and other required libraries, the best option is to use [Anaconda]("https://anaconda.org/").

### Import onnx model 

To install [ONNX]("https://onnx.ai/") and other required libraries, the best option is to use [Anaconda]("https://anaconda.org/") 

### Convert a model 

To convert an ONNX model to the ELL format use the following:

```
python onnx_import.py <path_to_onnx_model>
```
