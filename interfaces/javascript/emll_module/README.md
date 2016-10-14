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
