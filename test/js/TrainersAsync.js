"use strict";

let path = require('path');
console.log(path.relative('.', __dirname));

global.app_require = function(name) {
    return require(__dirname + '/' + name);
}

var tap = require('tap');
const EMLL = require("emll");
console.log("Loaded EMLL");

var lossarg = new EMLL.LossArguments();
lossarg.lossFunction = 0;

var sgdtrainarg = new EMLL.SGDIncrementalTrainerParameters();
sgdtrainarg.regularization = .75;

console.log("Done settings Loss and SGD Training Params ...");

var dataset = EMLL.GetDataset("../data/testData.txt");
console.log(dataset);
if(dataset)
{  
  var sgdIncrementalTrainer = EMLL.GetSGDIncrementalTrainer(21, lossarg, sgdtrainarg);
  var trainSetIterator = dataset.GetIterator();
  if(trainSetIterator)
  {
    console.log("Start training ....");
    sgdIncrementalTrainer.Update(trainSetIterator);
    console.log("Done training ....");
    tap.pass('Test SGDIncrementalTrianer');
    
    /*console.log("Start get predictor ....");
    let pred = sgdIncrementalTrainer.GetPredictor();
    if(pred)
    {
      console.log(pred);
      console.log("Predictor bias: " + pred.GetBias());
    }
    console.log("End get predictor ....");*/
  }
}

/*EMLL.GetDatasetAsync("../data/testData.txt", function(datasetasync)
{
  if(datasetasync)
  {
    console.log("Async Mode Size:" + datasetasync.NumExamples());
    tap.pass('Test GetDatasetAsync');
  }
});*/
