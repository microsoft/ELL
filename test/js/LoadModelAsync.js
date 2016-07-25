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
var modelsync = EMLL.LoadModelGraph("");
if(modelsync)
{
  console.log("Sync Mode Size:" + modelsync.Size());
  tap.pass('Test LoadModel syncronously');
}

EMLL.LoadModelAsync("" ,function(model)
{
  if(model)
  {
    console.log("ASync Mode Size:" + model.Size());
    tap.pass('Test LoadModelAsync asyncronously');
  }
});
