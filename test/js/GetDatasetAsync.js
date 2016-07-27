"use strict";

let path = require('path');
console.log(path.relative('.', __dirname));

global.app_require = function(name) {
    return require(__dirname + '/' + name);
}

var tap = require('tap');

console.log("#" + path.relative('.', __dirname));

//const EMLL = require("emll");
const EMLL = require("emll");


console.log("Loaded EMLL");
var dataset = EMLL.GetDataset("F:\\EMLLDepot\\EMLLModule\\test\\data\\testData.txt");
if(dataset)
{
  console.log("Sync Mode Size:" + dataset.NumExamples());
  tap.pass('Test GetDataset');
}

EMLL.GetDatasetAsync("F:\\EMLLDepot\\EMLLModule\\test\\data\\testData.txt", function(datasetasync)
{
  if(datasetasync)
  {
    console.log("Async Mode Size:" + datasetasync.NumExamples());
    tap.pass('Test GetDatasetAsync');
  }
});
