"use strict";

let path = require('path');
console.log(path.relative('.', __dirname));

global.app_require = function(name) {
    return require(__dirname + '/' + name);
}

var tap = require('tap');
const ELL = require("ell");
console.log("Loaded ELL");

var lossarg = new ell.LossArguments();
lossarg.lossFunction = 0;

var sgdtrainarg = new ell.SGDIncrementalTrainerParameters();
sgdtrainarg.regularization = .75;

console.log("Done settings Loss and SGD Training Params ...");

var dataset = ell.GetDataset("../data/testData.txt");
console.log(dataset);
if(dataset)
{  
  var sgdIncrementalTrainer = ell.GetSGDIncrementalTrainer(21, lossarg, sgdtrainarg);
  console.log(sgdIncrementalTrainer);
  
  var trainSetIterator = dataset.GetIterator();
  if(trainSetIterator)
  {
    console.log("Start training ....");
    sgdIncrementalTrainer.Update(trainSetIterator);
    console.log("Done training ....");
    tap.pass('Test SGDIncrementalTrianer');
    
    console.log("Start get predictor ....");
    let pred = sgdIncrementalTrainer.GetPredictor();
    if(pred)
    {
      console.log(pred);
      console.log("Linear Predictor weights: " + pred.GetWeights());
    }
    console.log("End get predictor ....");
    
    console.log(sgdIncrementalTrainer)
    
    console.log("Start training async....");    
    sgdIncrementalTrainer.UpdateAsync(trainSetIterator, function(result)
    {
      if(result)
      {
        console.log("Start get predictor ....");
        let pred = sgdIncrementalTrainer.GetPredictor();
        if(pred)
        {
          console.log(pred);
          console.log("Linear Predictor weights: " + pred.GetWeights());
        }
        console.log("End get predictor ....");           
      }
    });
    console.log("Done training async ....");
  }
}

/*ell.GetDatasetAsync("../data/testData.txt", function(datasetasync)
{
  if(datasetasync)
  {
    console.log("Async Mode Size:" + datasetasync.NumExamples());
    tap.pass('Test GetDatasetAsync');
  }
});*/
