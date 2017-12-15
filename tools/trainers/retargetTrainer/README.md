## retarget

Example usage (single class / binary prediction):
```shell
retargetTrainer --inputModelFilename trainedModel.ell  --targetPortElements 1109.output --inputDataFilename singleClass.gsdf --outputModelFilename retargetedModel.ell
```

Example usage (multi class prediction):
```shell
retargetTrainer --inputModelFilename trainedModel.ell  --targetPortElements 1109.output --inputDataFilename multiClass.gsdf --multiClass true --outputModelFilename retargetedModel.ell
```

Help:

```
Usage: retargetTrainer [options]

        --inputModelFilename (-imf) []    Name of the pre-trained ELL model file (e.g. model1.ell) that will be used as a featurizer for a linear predictor
        --outputModelFilename (-omf) []   Name of the output file that will hold the saved retargeted model (e.g. retargetedModel.ell)
        --refineIterations (-ri) [1]      If cutting the neural network using a node id, specifies the maximum number of refinement iterations
        --targetPortElements (-tpe) []    The port elements of the pre-trained model to use as input to the subsequent linear predictor e.g. "1115.output" to use the full output from Node 1115
        --removeLastLayers (-rem) [0]     Instead of using a node id, a neural network model can be retargeted by removing the last N layers
        --inputDataFilename (-idf) []     Path to the input dataset file
        --multiClass (-mc) [false]        Indicates whether the input dataset is multi-class or binary.
        --normalize (-n) [false]          Perform sparsity-preserving normalization
        --regularization (-r) [1]         The L2 regularization parameter
        --desiredPrecision (-de) [1e-05]  The desired duality gap at which to stop optimizing
        --maxEpochs (-me) [1000]          The maximum number of optimization epochs to run
        --permute (-p) [true]             Whether or not to randomly permute the training data before each epoch
        --randomSeedString (-seed) [ABCDEFG]  The random seed string
        --verbose (-v) [false]            Print diagnostic output during the execution of the tool to stdout
        --lossFunction (-lf) [log]        Choice of loss function  {squared | log | smoothHinge}
        --help (-h) [false]               Print help and exit```
```

### What is this tool used for?
The `retargetTrainer` tool trains a new ELL model which is the combination of an existing model and a linear predictor - or set of linear predictors - trained with Stochastic Dual Coordinate Ascent (SDCA). Using this method, a generalized pre-trained model (e.g. a 1000 class image classifier) can be specialized or retargeted to:
* work on new classes that the generalized model wasn't trained on
* achieve high accuracy by reducing the number of predicted classes
* speed up the model by removing expensive sections of the model and replacing with simple linear predictors

The resulting model can be compiled or wrapped as normal using the [wrap](../../wrap/README.md) tool.

#### --targetPortElements and --refineIterations
Use the '--targetPortElements' option to specify which node's output should be the input of the linear predictor(s). This has the effect of splicing the linear predictor(s) onto the pre-trained model after the specified node.

Some example Port elements strings could be:
* nodeId.output
  * Takes the full output from Node specified by NodeId
* {nodeId1.output,nodeId2.output}
  * Takes the full output from Node1 concatenated with the full output from Node2
* nodeId.output[5:20]
  * Takes a range of the output from Node specified by NodeId, starting with element at index 5 and up to, but not including element at index 20.

The nodeId can be retrieved using the `print` tool. Note that a model's nodes change after every refinement iteration. Using the same '--refineIterations' value in both `print` and `retargetTrainer` tools will ensure that the id matches.

#### --removeLastLayers
As a convenience, if the pretrained model is a neural network, the model can be retargeted by removing the last N layers. This has the same effect as using the node id of the node which precedes the Nth layer.

#### --multiClass
By default, this tool trains a single binary class linear predictor. If the dataset represents a multi-class dataset, set this option to true. The tool will then train a set of linear predictors in a One Versus Rest (OVR) strategy. The resulting linear predictors are then combined into a more efficient set of operational nodes that are functionally equivalent to multiple linear predictors.

### Example Output (single class)
```shell
retargetTrainer --inputModelFilename trainedModel.ell  --targetPortElements 1109.output --inputDataFilename singleClass.gsdf --outputModelFilename retargetedModel.ell --verbose

Loading model from trainedModel.ell
Found node 1109
Loading data ...
Created linear trainer ...
Training ...
        Primal Objective        Dual Objective  Duality gap
        0.052837                0.045591        0.007246
        0.048969                0.048139        0.000830
        0.048395                0.048325        0.000070
        0.048353                0.048345        0.000008
Final duality Gap: 0.000008

ErrorRate       Precision       Recall          F1-Score        AUC             MeanLoss
1.000000        0.000000        0.000000        0.000000        0.000000        0.693147
0.000000        1.000000        1.000000        1.000000        1.000000        0.016231

Training completed successfully.
```

### Example Output (multi class)
```shell
retargetTrainer --inputModelFilename trainedModel.ell  --targetPortElements 1109.output --inputDataFilename multiClass.gsdf --multiClass true --outputModelFilename retargetedModel.ell --verbose

Loading model from trainedModel.ell
Found node 1109
Loading data ...

=== Training binary classifier for class 0 vs Rest ===
Created linear trainer ...
Training ...
        Primal Objective        Dual Objective  Duality gap
        0.050785                0.037489        0.013296
        0.043370                0.042257        0.001112
        0.042729                0.042655        0.000073
        0.042689                0.042678        0.000011
        0.042681                0.042680        0.000001
Final duality Gap: 0.000001

ErrorRate       Precision       Recall          F1-Score        AUC             MeanLoss
1.000000        0.000000        0.000000        0.000000        0.000000        0.693147
0.000000        1.000000        1.000000        1.000000        1.000000        0.020983

Training completed successfully.

=== Training binary classifier for class 1 vs Rest ===
Created linear trainer ...
Training ...
        Primal Objective        Dual Objective  Duality gap
        0.064290                0.049385        0.014905
        0.053756                0.052514        0.001242
        0.053126                0.052985        0.000141
        0.053033                0.053022        0.000011
        0.053026                0.053025        0.000001
Final duality Gap: 0.000001

ErrorRate       Precision       Recall          F1-Score        AUC             MeanLoss
1.000000        0.000000        0.000000        0.000000        0.000000        0.693147
0.000000        1.000000        1.000000        1.000000        1.000000        0.025060

Training completed successfully.

=== Training binary classifier for class 2 vs Rest ===
Created linear trainer ...
Training ...
        Primal Objective        Dual Objective  Duality gap
        0.088536                0.056823        0.031713
        0.065643                0.062362        0.003282
        0.064207                0.063106        0.001101
        0.063326                0.063191        0.000135
        0.063240                0.063214        0.000027
        0.063218                0.063216        0.000001
Final duality Gap: 0.000001

ErrorRate       Precision       Recall          F1-Score        AUC             MeanLoss
1.000000        0.000000        0.000000        0.000000        0.000000        0.693147
0.000000        1.000000        1.000000        1.000000        1.000000        0.035093

Training completed successfully.

=== Training binary classifier for class 3 vs Rest ===
Created linear trainer ...
Training ...
        Primal Objective        Dual Objective  Duality gap
        0.088045                0.049776        0.038270
        0.060388                0.056094        0.004294
        0.057985                0.056768        0.001217
        0.056956                0.056885        0.000071
        0.056927                0.056908        0.000019
        0.056913                0.056911        0.000002
Final duality Gap: 0.000002

ErrorRate       Precision       Recall          F1-Score        AUC             MeanLoss
1.000000        0.000000        0.000000        0.000000        0.000000        0.693147
0.000000        1.000000        1.000000        1.000000        1.000000        0.023844

Training completed successfully.

=== Training binary classifier for class 4 vs Rest ===
Created linear trainer ...
Training ...
        Primal Objective        Dual Objective  Duality gap
        0.093282                0.044908        0.048375
        0.055631                0.050771        0.004860
        0.052815                0.052071        0.000744
        0.052239                0.052178        0.000061
        0.052223                0.052194        0.000029
        0.052198                0.052196        0.000001
Final duality Gap: 0.000001

ErrorRate       Precision       Recall          F1-Score        AUC             MeanLoss
1.000000        0.000000        0.000000        0.000000        0.000000        0.693147
0.000000        1.000000        1.000000        1.000000        1.000000        0.024197

Training completed successfully.
```

Note that in the multi-class case, a binary class linear predictor is trained for each One Versus Rest (OVR) case.