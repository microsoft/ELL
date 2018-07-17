# Note:
This repo is an experimental PR. It's not ready for release yet, need extensive review and cleanup 

### Environment setup
You should have both [ONNX]("https://onnx.ai/") and [ELL]("https://microsoft.github.io/ELL") installed. 

To install ELL follow the instruction for your specific os ([Windows]("https://github.com/Microsoft/ELL/blob/master/INSTALL-Windows.md"), [Ubuntu Linux]("https://github.com/Microsoft/ELL/blob/master/INSTALL-Ubuntu.md"), [macOS]("https://github.com/Microsoft/ELL/blob/master/INSTALL-Mac.md") ) 

To install [ONNX]("https://onnx.ai/") and other required library, the best option is to use the [Anaconda]("https://anaconda.org/") 
and create an Environment using the Environment.yml file found in this directory

### Convert a model by typping
`python onnx_importer.py <path_to_onnx_model>`