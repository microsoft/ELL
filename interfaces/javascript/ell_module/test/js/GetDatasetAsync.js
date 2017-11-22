"use strict";

let path = require('path');
console.log(path.relative('.', __dirname));

global.app_require = function(name) {
    return require(__dirname + '/' + name);
}

var tap = require('tap');

console.log("#" + path.relative('.', __dirname));

const ELL = require("ell");

console.log("Loaded ELL");
var dataset = ell.GetDataset("../data/testData.txt");
console.log(dataset);
if(dataset)
{
  console.log("Sync Mode Size:" + dataset.NumExamples());
  tap.pass('Test GetDataset');
  
  var dataiter = dataset.GetIterator();
  console.log("Sync Itearator IsValid:" + dataiter.IsValid());
  console.log("Sync Itearator no. of iterations left:" + dataiter.NumIteratesLeft());
}

ell.GetDatasetAsync("../data/testData.txt", function(datasetasync)
{
  if(datasetasync)
  {
    console.log("Async Mode Size:" + datasetasync.NumExamples());
    tap.pass('Test GetDatasetAsync');
  }
});
