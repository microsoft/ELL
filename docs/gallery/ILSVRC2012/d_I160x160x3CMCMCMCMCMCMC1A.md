---
layout: default
title: 160x160x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/d_I160x160x3CMCMCMCMCMCMC1A
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 160x160x3 Convolutional Neural Network (57.12% top 1 accuracy, 80.22% top 5 accuracy, 0.62s/frame on Raspberry Pi 3)

|=
| Download | [d_I160x160x3CMCMCMCMCMCMC1A.ell.zip](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I160x160x3CMCMCMCMCMCMC1A/d_I160x160x3CMCMCMCMCMCMC1A.ell.zip)
|-
| Accuracy | ILSVRC2012: 80.22% (Top 5), 57.12% (Top 1) 
|-
| Performance | Raspberry Pi 3 (Raspbian) @ 700MHz: 0.62s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 800MHz: @pi3_64_SECONDS_PER_FRAME<br>DragonBoard 410c @ 1.2GHz: 0.40s/frame
|-
| Uncompressed Size | 91MB
|-
| Input | 160 x 160 x {R,G,B}
|-
| Architecure | Minus :  160x160x3  ->  162x162x3  input padding 0  output padding 1<br>Convolution(LeakyReLU) :  162x162x3  ->  160x160x16  input padding 1  output padding 0<br>BatchNormalization :  160x160x16  ->  160x160x16  input padding 0  output padding 0<br>MaxPooling :  160x160x16  ->  82x82x16  input padding 0  output padding 1<br>Convolution(LeakyReLU) :  82x82x16  ->  80x80x64  input padding 1  output padding 0<br>BatchNormalization :  80x80x64  ->  80x80x64  input padding 0  output padding 0<br>MaxPooling :  80x80x64  ->  42x42x64  input padding 0  output padding 1<br>Convolution(LeakyReLU) :  42x42x64  ->  40x40x64  input padding 1  output padding 0<br>BatchNormalization :  40x40x64  ->  40x40x64  input padding 0  output padding 0<br>MaxPooling :  40x40x64  ->  22x22x64  input padding 0  output padding 1<br>Convolution(LeakyReLU) :  22x22x64  ->  20x20x128  input padding 1  output padding 0<br>BatchNormalization :  20x20x128  ->  20x20x128  input padding 0  output padding 0<br>MaxPooling :  20x20x128  ->  12x12x128  input padding 0  output padding 1<br>Convolution(LeakyReLU) :  12x12x128  ->  10x10x256  input padding 1  output padding 0<br>BatchNormalization :  10x10x256  ->  10x10x256  input padding 0  output padding 0<br>MaxPooling :  10x10x256  ->  7x7x256  input padding 0  output padding 1<br>Convolution(LeakyReLU) :  7x7x256  ->  5x5x512  input padding 1  output padding 0<br>BatchNormalization :  5x5x512  ->  5x5x512  input padding 0  output padding 0<br>MaxPooling :  5x5x512  ->  5x5x512  input padding 0  output padding 1<br>Convolution(LeakyReLU) :  5x5x512  ->  3x3x1024  input padding 1  output padding 0<br>BatchNormalization :  3x3x1024  ->  3x3x1024  input padding 0  output padding 0<br>Convolution :  3x3x1024  ->  3x3x1000  input padding 0  output padding 0<br>AveragePooling :  3x3x1000  ->  1x1x1000  input padding 0  output padding 0<br>Softmax :  1x1x1000  ->  1x1x1  input padding 0  output padding 0<br>
|-
| Output | [ILSVRC2012 1000 classes](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/ILSVRC2012_labels.txt)
|-
| Notes | Trained by Chuck Jacobs using CNTK 2.1
|=
