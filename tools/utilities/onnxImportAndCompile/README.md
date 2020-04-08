## onnxImportAndCompile.py

This tool imports an ONNX file, converts it to .ell, and calls wrap.py. 

## Prerequisites

The tool relies on the following python libraries:
* argparse
* validators

## Usage

```
python onnxImportAndCompile.py [--onnx ONNX_FILE]
                               [--lang OUTPUT_LANGUAGE]
                               [--target TARGET_LIST] 
```

* **ONNX_FILE** - ONNX file path to be imported. This can be a local path or a URL
* **OUTPUT_LANGUAGE** - Output language for wrap script (examples: 'cpp' or 'python')
* **TARGET_LIST** - The target platform(s) for the wrap script, as a comma-separted list (i.e. 'pi0,pi3')
