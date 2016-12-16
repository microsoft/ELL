'use strict';

var ellLib = null;

// darwin, freebas, linux, sunos, win32

if(process.platform == 'win32') {
    ellLib = require('./dist/win/build/Release/ell')
} else if (process.platform == 'darwin') {
    ellLib = require('./dist/mac/build/Release/ell')
} else if (process.platform == 'linux') {
    ellLib = require('./dist/linux/build/Release/ell')
}

module.exports = ellLib;
