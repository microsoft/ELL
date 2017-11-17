var fs = require('fs');
var libxslt = require('libxslt');

var xsltFilename = process.argv[2];
var documentFilename = process.argv[3];
var outputFilename = process.argv[4];

libxslt.parseFile(xsltFilename, function (err, stylesheet) {
    if (err) {
        console.log("Error during XSLT parsing:");
        console.log(err);
        return;
    }

    stylesheet.applyToFile(documentFilename, function (err, result) {
        // err contains any error from parsing the document or applying the stylesheet 
        // result is a string containing the result of the transformation 
        fs.writeFile(outputFilename, result);
    });
});
