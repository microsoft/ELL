# Embedded Learning Library (ELL)

## Overview

ELL is an early preview of our work on building and deploying machine-learned pipelines onto resource-constrained embedded platforms. 
This project is a work in progress and so expect it to change rapidly.   

## License

See LICENSE.txt.

## Code of conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information on this code of conduct, see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

## To build the module

The ELL module should automatically get built when you build the ELL solution.

Note: If you get an error trying to build the module, and you've updated to Visual Studio 2015 Update 3, you need up update node-gyp from v3.2.1 to v3.4.0 or newer. Do the following:

1.	Go to your folder where npm is installed, e.g.: C:\Program Files\nodejs\node_modules\npm
2.	Open package.json
3.	Remove entry for node-gyp in bundleDependencies
4.	Bump version number to 3.4.0 for node-gyp in dependencies
5.	Run "npm install" in this directory to install node-gyp@3.4.0 (which fixes the problem)
6.  Alternately, this might work: `npm install -g node-gyp@3.4.0`

## To manually build and install the module

1. Build a Release build of ELL
2. Build the ELL module from npm:
  * On Windows: `npm run build:win`
  * On Mac OS: `npm run build:mac`
  * On Linux: `npm run build:linux`

## To run the tests

1. Make sure you build the module (follow the required steps from the previous section) from the root ELLModule folder.
2. cd test/js
3. run npm install. This should install the pre-requisites to run the tests for ELL APIs
4. Run `node <filename>.js` to run the test.

_Note: The tests currently only work in Windows_
