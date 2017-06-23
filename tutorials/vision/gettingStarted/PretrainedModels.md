# Pre-trained models that work well with the tutorial
The models below have been trained on the 1000-class [ImageNet Challenge](http://image-net.org/challenges/LSVRC/2015/index) data set. For smaller devices, we recommend the Darknet reference model, since it is fast and reasonably accurate.

It is worth trying different models, to see which one works best for your particular scenario. The following pre-trained models have been tested with this tutorial and can be a good place to start.

## CNTK ImageNet Models
CNTK models can be directly imported using the *cntk_to_ell* Python module (found in tools/importers/CNTK) e.g.

    predictor = cntk_to_ell.predictor_from_cntk_model("someModel.model")

### VGG16 Model
A CNTK VGG-16 model trained on ImageNet can be found [here](https://github.com/Microsoft/CNTK/tree/master/Examples/Image/Classification/VGG), using your browser.
Or you can download directly using the following commandline:

    curl -O https://www.cntk.ai/Models/Caffe_Converted/VGG16_ImageNet_Caffe.model

### VGG19 Model
A CNTK VGG-19 model trained on ImageNet can be found [here](https://github.com/Microsoft/CNTK/tree/master/Examples/Image/Classification/VGG), using your browser.
Or you can download directly using the following commandline:

    curl -O https://www.cntk.ai/Models/Caffe_Converted/VGG19_ImageNet_Caffe.model

## Darknet ImageNet models
Darknet models can be directly imported using the *darknet_to_ell* Python module (found in tools/importers/Darknet) e.g.

    predictor = darknet_to_ell.predictor_from_darknet_model("someModel.cfg", "someModel.weights")

### Reference model
The Darknet reference model trained on ImageNet can be found [here](https://pjreddie.com/darknet/imagenet/#reference), using your browser.
Or you can download directly using the following commandline:

    curl -O https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/darknet.cfg
    curl -O https://pjreddie.com/media/files/darknet.weights

### VGG16 Model
A Darknet VGG-16 model trained on ImageNet can be found [here](https://pjreddie.com/darknet/imagenet/#vgg), using your browser.
Or you can download directly using the following commandline:

    curl -O https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/vgg-16.cfg
    curl -O https://pjreddie.com/media/files/vgg-16.weights

### Extraction Model
The Darknet Extraction model trained on ImageNet can be found [here](https://pjreddie.com/darknet/imagenet/#extraction), using your browser.
Or you can download directly using the following commandline:

    curl -O https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/extraction.cfg
    curl -O https://pjreddie.com/media/files/extraction.weights

### Darknet19 Model
The Darknet 19 model trained on ImageNet can be found [here](https://pjreddie.com/darknet/imagenet/#Darknet19), using your browser.
Or you can download directly using the following commandline:

    curl -O https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/darknet19.cfg
    curl -O https://pjreddie.com/media/files/darknet19.weights
