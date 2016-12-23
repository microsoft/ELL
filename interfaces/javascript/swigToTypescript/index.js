var libxslt = require('libxslt');

var xsltFilename = './generate-d-ts.xslt';
var documentFilename = '../../../build/interfaces/xml/ELLXML_wrap.xml';
libxslt.parseFile(xsltFilename, function (err, stylesheet) {
    if (err) {
        console.log(err);
        return;
    }
    var params = {
        MyParam: 'my value'
    };

    // 'params' parameter is optional 
    stylesheet.applyToFile(documentFilename, params, function (err, result) {
        // err contains any error from parsing the document or applying the stylesheet 
        // result is a string containing the result of the transformation 
        console.log("Transformed document:");
        console.log(result);
    });
});
