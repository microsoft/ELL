'use strict';

let path = require('path');
console.log(path.relative('.', __dirname));

global.app_require =
    function(name) {
  return require(__dirname + '/' + name);
}

var tap = require('tap');
const EMLL = require('emll');
console.log('Loaded EMLL');

// `array` is a list of lists (a javascript jagged array)
function ToEMLLVector(array) {
  let result = new EMLL.DoubleVectorVector();
  for (var rowIndex = 0; rowIndex < array.length; rowIndex++) {
    let row = array[rowIndex];
    let newRow = new EMLL.DoubleVector();
    for (var colIndex = 0; colIndex < row.length; colIndex++) {
      newRow.add(row[colIndex]);
    }
    result.add(newRow);
  }
  return result;
}

function PrintResults(map, data) {
  for (var rowIndex = 0; rowIndex < data.size(); rowIndex++) {
    var result = map.Compute(data.get(rowIndex));
    let resultStr = '';
    for (var colIndex = 0; colIndex < result.size(); colIndex++) {
      resultStr += result.get(colIndex).toString() + ' ';
    }
    console.log(resultStr);
  }
}


let prototype = ToEMLLVector([[1, 2, 3], [4, 5, 6], [7, 8, 9]]);
let map = EMLL.GenerateDTWClassifier(prototype);
console.log('Generated classifier');

let code = map.GetCodeString();
console.log('Code:');
console.log(code);

let testData = ToEMLLVector([
  [1, 2, 3], [4, 5, 6], [7, 8, 9], [3, 2, 1], [5, 4, 3], [4, 5, 6], [7, 8, 9]
]);

console.log('Compute:');
PrintResults(map, testData);
