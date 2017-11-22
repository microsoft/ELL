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
var modelsync = ell.LoadModel("[1]");
if(modelsync)
{
  console.log("Sync Mode Size:" + modelsync.Size());
  tap.pass('Test LoadModel syncronously');
}

ell.LoadModelAsync("[1]" ,function(model)
{
  if(model)
  {
    console.log("ASync Mode Size:" + model.Size());
    tap.pass('Test LoadModelAsync asyncronously');
  }
});
