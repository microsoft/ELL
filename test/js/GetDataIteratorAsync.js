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
var dataiter = EMLL.GetDataIterator("../data/testData.txt");
console.log(dataiter);
/*var dataiter = EMLL.GetDataIterator("../data/testData.txt");
if(dataiter)
{
  console.log("Sync Mode Size:" + dataiter.HasSize());
  tap.pass('Test GetDataIterator');
}*/

EMLL.GetDataIteratorAsync("../data/testData.txt", function(dataiterasync)
{
  if(dataiterasync)
  {
    console.log("Async Mode Size:" + dataiterasync.NumIteratesLeft());
    tap.pass('Test GetDataIteratorAsync');
  }
});
