---
layout: default
title: 192x192x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/d_I192x192x3CMCMCMCMCMCMC1A
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 192x192x3 Convolutional Neural Network (59.01% top 1 accuracy, 81.74% top 5 accuracy, 0.82s/frame on Raspberry Pi 3)

|=
| Download | [d_I192x192x3CMCMCMCMCMCMC1A.ell.zip](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I192x192x3CMCMCMCMCMCMC1A/d_I192x192x3CMCMCMCMCMCMC1A.ell.zip)
|-
| Accuracy | ILSVRC2012: 81.74% (Top 5), 59.01% (Top 1) 
|-
| Performance | Raspberry Pi 3 (Raspbian) @ 700MHz: 0.82s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 600MHz: 0.84s/frame<br>DragonBoard 410c @ 1.2GHz: 0.46s/frame
|-
| Uncompressed Size | 91MB
|-
| Input | 192 x 192 x {R,G,B}
|-
| Architecure | Minus :  192x192x3  ->  192x192x3  input padding 0  output padding 0<br>BatchNormalization :  192x192x3  ->  194x194x3  input padding 0  output padding 1<br>Convolution(ReLU) :  194x194x3  ->  192x192x16  input padding 1  output padding 0<br>MaxPooling :  192x192x16  ->  96x96x16  input padding 0  output padding 0<br>BatchNormalization :  96x96x16  ->  98x98x16  input padding 0  output padding 1<br>Convolution(ReLU) :  98x98x16  ->  96x96x64  input padding 1  output padding 0<br>MaxPooling :  96x96x64  ->  48x48x64  input padding 0  output padding 0<br>BatchNormalization :  48x48x64  ->  50x50x64  input padding 0  output padding 1<br>Convolution(ReLU) :  50x50x64  ->  48x48x64  input padding 1  output padding 0<br>MaxPooling :  48x48x64  ->  24x24x64  input padding 0  output padding 0<br>BatchNormalization :  24x24x64  ->  26x26x64  input padding 0  output padding 1<br>Convolution(ReLU) :  26x26x64  ->  24x24x128  input padding 1  output padding 0<br>MaxPooling :  24x24x128  ->  12x12x128  input padding 0  output padding 0<br>BatchNormalization :  12x12x128  ->  14x14x128  input padding 0  output padding 1<br>Convolution(ReLU) :  14x14x128  ->  12x12x256  input padding 1  output padding 0<br>MaxPooling :  12x12x256  ->  6x6x256  input padding 0  output padding 0<br>BatchNormalization :  6x6x256  ->  8x8x256  input padding 0  output padding 1<br>Convolution(ReLU) :  8x8x256  ->  6x6x512  input padding 1  output padding 0<br>MaxPooling :  6x6x512  ->  3x3x512  input padding 0  output padding 0<br>BatchNormalization :  3x3x512  ->  5x5x512  input padding 0  output padding 1<br>Convolution(ReLU) :  5x5x512  ->  3x3x1024  input padding 1  output padding 0<br>BatchNormalization :  3x3x1024  ->  3x3x1024  input padding 0  output padding 0<br>Convolution(ReLU) :  3x3x1024  ->  3x3x1000  input padding 0  output padding 0<br>AveragePooling :  3x3x1000  ->  1x1x1000  input padding 0  output padding 0<br>Softmax :  1x1x1000  ->  1x1x1  input padding 0  output padding 0<br>
|-
| Output | [ILSVRC2012 1000 classes](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/ILSVRC2012_labels.txt)
|-
| Notes | Trained by Chuck Jacobs using CNTK 2.1
|=
