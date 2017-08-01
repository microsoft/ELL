# Importers

## Prerequisites
ELL provides a number of importer libraries that can consume a model from another framework using Python.

In order to use one of these importers, be sure to first install the ELL python bindings.
- If you do not already have the ELL python bindings installed, follow the instructions in [Interfaces](../../interfaces/README.md) to build them.
- Copy the resulting ELL Python API files to your Python site-packages
  - Main python library file for ELL Python APIs. This is the `ELL.py` file.
  - Backing native modules for ELL Python APIs. `_ELL.so` on Linux and Mac, or `_ELL.pyd` on Windows

## CNTK to ELL
The [CNTK](CNTK/doc/README.md) folder contains a simple importer for previously trained CNTK models. You can create an ELL Neural Network Predictor from Python using the `predictor_from_cntk_model` function e.g.

        from cntk_to_ell import predictor_from_cntk_model
        predictor = predictor_from_cntk_model('myCntkModel.dnn')

The CNTK importer library works on models saved with CNTK 2.0 and later. Models from version 1.x are not supported. These models should be loaded and re-saved in CNTK 2.0 or later.

## Darknet to ELL
The `darknet_to_ell.py` file contains a simple importer for previously trained Darknet models. You can create an ELL Neural Network Predictor from Python using the `predictor_from_cntk_model` function e.g.

        from darknet_to_ell import predictor_from_darknet_model
        predictor = predictor_from_darknet_model('myDarknet.cfg', 'myDarknet.weights')
