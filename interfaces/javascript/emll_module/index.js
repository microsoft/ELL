'use strict';

var emllLib = null;

// darwin, freebas, linux, sunos, win32

if(process.platform == 'win32') {
    emllLib = require('./dist/win/build/Release/emll')
} else if (process.platform == 'darwin') {
    emllLib = require('./dist/mac/build/Release/emll')
} else if (process.platform == 'linux') {
    emllLib = require('./dist/linux/build/Release/emll')
}

module.exports = emllLib;
