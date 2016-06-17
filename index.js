'use strict';

var emllLib = null;

// darwin, freebas, linux, sunos, win32

if(process.platform == 'win32') {
    emllLib = require('./win/build/Release/emll')
} else if (process.platform == 'darwin') {
    emllLib = require('./osx/build/Release/emll')
} else if (process.platform == 'linux') {
    emllLib = require('./linux/build/Release/emll')
}

module.exports = emllLib;
