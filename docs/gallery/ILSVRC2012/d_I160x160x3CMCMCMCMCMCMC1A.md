---
layout: default
title: 160x160x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/d_I160x160x3CMCMCMCMCMCMC1A
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 160x160x3 Convolutional Neural Network (42.40% top 1 error, 19.61% top 5 error, 0.66s/frame)

|=
| Download | [d_I160x160x3CMCMCMCMCMCMC1A.ell.zip](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I160x160x3CMCMCMCMCMCMC1A/d_I160x160x3CMCMCMCMCMCMC1A.ell.zip)
|-
| Accuracy | ILSVRC2012: 80.39% (Top 5), 57.60% (Top 1) 
|-
| Performance | Raspberry Pi 3 (Raspbian) @ 700MHz: 0.66s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 800MHz: <br>DragonBoard 410c @ 1.2GHz:
|-
| Uncompressed Size | 91MB
|-
| Input | 160 x 160 x {R,G,B}
|-
| Architecure | Minus :  160x160x3  ->  160x160x3  input padding 0  output padding 0<br>BatchNormalization :  160x160x3  ->  162x162x3  input padding 0  output padding 1<br>Convolution(ReLU) :  162x162x3  ->  160x160x16  input padding 1  output padding 0<br>MaxPooling :  160x160x16  ->  80x80x16  input padding 0  output padding 0<br>BatchNormalization :  80x80x16  ->  82x82x16  input padding 0  output padding 1<br>Convolution(ReLU) :  82x82x16  ->  80x80x64  input padding 1  output padding 0<br>MaxPooling :  80x80x64  ->  40x40x64  input padding 0  output padding 0<br>BatchNormalization :  40x40x64  ->  42x42x64  input padding 0  output padding 1<br>Convolution(ReLU) :  42x42x64  ->  40x40x64  input padding 1  output padding 0<br>MaxPooling :  40x40x64  ->  20x20x64  input padding 0  output padding 0<br>BatchNormalization :  20x20x64  ->  22x22x64  input padding 0  output padding 1<br>Convolution(ReLU) :  22x22x64  ->  20x20x128  input padding 1  output padding 0<br>MaxPooling :  20x20x128  ->  10x10x128  input padding 0  output padding 0<br>BatchNormalization :  10x10x128  ->  12x12x128  input padding 0  output padding 1<br>Convolution(ReLU) :  12x12x128  ->  10x10x256  input padding 1  output padding 0<br>MaxPooling :  10x10x256  ->  5x5x256  input padding 0  output padding 0<br>BatchNormalization :  5x5x256  ->  7x7x256  input padding 0  output padding 1<br>Convolution(ReLU) :  7x7x256  ->  5x5x512  input padding 1  output padding 0<br>MaxPooling :  5x5x512  ->  3x3x512  input padding 0  output padding 0<br>BatchNormalization :  3x3x512  ->  5x5x512  input padding 0  output padding 1<br>Convolution(ReLU) :  5x5x512  ->  3x3x1024  input padding 1  output padding 0<br>BatchNormalization :  3x3x1024  ->  3x3x1024  input padding 0  output padding 0<br>Convolution(ReLU) :  3x3x1024  ->  3x3x1000  input padding 0  output padding 0<br>AveragePooling :  3x3x1000  ->  1x1x1000  input padding 0  output padding 0<br>Softmax :  1x1x1000  ->  1x1x1  input padding 0  output padding 0<br>
|-
| Output | [ILSVRC2012 1000 classes](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/ImageNetLabels.txt)
|-
| Notes | Trained by Chuck Jacobs using CNTK 2.1
|=
