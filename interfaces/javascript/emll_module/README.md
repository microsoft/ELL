# Embedded Machine Learning Library (EMLL)

## Overview

EMLL is an early preview of our work on building and deploying machine-learned pipelines onto resource-constrained embedded platforms. 
This project is a work in progress and we expect it to change rapidly. At this early stage, we recommend not to take any dependencies on our work.    

## License

See LICENSE.txt.

## Code of conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information on this code of conduct, see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

## Installing

See INSTALL.md.


To build and install the module
-------------------------------

1. Make sure the EMLL repository exists in a directory called "EMLL", and is a sibling directory to this one.
2. Build a Release build of EMLL
3. Build the EMLL module from npm:
  * On Windows: `npm run buildwin`
  * On Mac OS: `npm run buildmac`
  * On Linux: `npm run buildlinux`

1. Make sure you build the module (follow the required steps from the previous section) from the root EMLLModule folder.
2. cd test/js
3. run npm install. This should install the pre-requisites to run the tests for EMLL APIs
4. Run "node <filename>.js" to run the test.

_Note: The tests currently only work in Windows_


If you get an error trying to build the module, and you've updated to Visual Studio 2015 Update 3, you need up update node-gyp from v3.2.1 to v3.4.0 or newer. Do the following:

1.	Go to your folder where npm is installed, e.g.: C:\Program Files\nodejs\node_modules\npm
2.	Open: package.json
3.	Remove entry for node-gyp in bundleDependencies
4.	Bump version number to 3.4.0 for node-gyp in dependencies
5.	Make a npm i in this directory to install node-gyp@3.4.0 to fix the problem

(from <http://stackoverflow.com/questions/38149603/npm-install-fails-with-error-c2373-with-vs2015-update-3>)

6.  npm install -g node-gyp@3.4.0
