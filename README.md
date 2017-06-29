# Embedded Learning Library


The Embedded Learning Library (ELL) allows you to build and deploy machine-learned pipelines onto embedded platforms, like Raspberry Pis, Arduinos, Micro:bits, and other microcontrollers. The deployed machine learning model runs on the device, disconnected from the cloud. Our APIs can be used either from C++ or Python. 

This project has been developed by a team of researchers at Microsoft Research. It's a work in progress, and we expect it to change rapidly, including breaking API changes. Despite this code churn, we welcome you to try it and give us feedback! 

A good place to start is the [tutorial](tutorials/vision/gettingStarted/README.md),
which allows you to do image recognition on a Raspberry Pi with a web cam, disconnected from the cloud. The  software you deploy to the Pi will recognize a variety of common objects on camera and print a label for the recognized object on the Pi's screen.

![coffee mug](tutorials/vision/gettingStarted/coffeemug.jpg)


## License

See [LICENSE.txt](LICENSE.txt). 

## Code of conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information on this code of conduct, see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

## Build and Installation Instructions

* On [Windows](INSTALL-Windows.md)
* On [Ubuntu Linux](INSTALL-Ubuntu.md)
* On [Mac OS X](INSTALL-Mac.md)

## Additional Documentation

* [Design overview of `data` library](./libraries/data/doc/README.md)
* [Design overview of `math` library](./libraries/math/doc/README.md)
* [Design overview of `model` and `nodes` libraries](./libraries/model/doc/README.md)
* [Definition of the generalized sparse data format](./libraries/data/doc/GeneralizedSparseFormat.md)
* [Description of the training algorithms implemented in `trainers`](./libraries/trainers/doc/README.md)




