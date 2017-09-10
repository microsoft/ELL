---
layout: default
title: 256x256x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/d_I256x256x3CMCMCMCMCMCMC1A
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 256x256x3 Convolutional Neural Network (38.93% top 1 error, 16.82% top 5 error, 1.61s/frame on Raspberry Pi 3)

|=
| Download | [d_I256x256x3CMCMCMCMCMCMC1A.ell.zip](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I256x256x3CMCMCMCMCMCMC1A/d_I256x256x3CMCMCMCMCMCMC1A.ell.zip)
|-
| Accuracy | ILSVRC2012: 83.18% (Top 5), 61.07% (Top 1) 
|-
| Performance | Raspberry Pi 3 (Raspbian) @ 700MHz: 1.61s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 800MHz: <br>DragonBoard 410c @ 1.2GHz:
|-
| Uncompressed Size | 91MB
|-
| Input | 256 x 256 x {R,G,B}
|-
| Architecure | Minus :  256x256x3  ->  256x256x3  input padding 0  output padding 0<br>BatchNormalization :  256x256x3  ->  258x258x3  input padding 0  output padding 1<br>Convolution(ReLU) :  258x258x3  ->  256x256x16  input padding 1  output padding 0<br>MaxPooling :  256x256x16  ->  128x128x16  input padding 0  output padding 0<br>BatchNormalization :  128x128x16  ->  130x130x16  input padding 0  output padding 1<br>Convolution(ReLU) :  130x130x16  ->  128x128x64  input padding 1  output padding 0<br>MaxPooling :  128x128x64  ->  64x64x64  input padding 0  output padding 0<br>BatchNormalization :  64x64x64  ->  66x66x64  input padding 0  output padding 1<br>Convolution(ReLU) :  66x66x64  ->  64x64x64  input padding 1  output padding 0<br>MaxPooling :  64x64x64  ->  32x32x64  input padding 0  output padding 0<br>BatchNormalization :  32x32x64  ->  34x34x64  input padding 0  output padding 1<br>Convolution(ReLU) :  34x34x64  ->  32x32x128  input padding 1  output padding 0<br>MaxPooling :  32x32x128  ->  16x16x128  input padding 0  output padding 0<br>BatchNormalization :  16x16x128  ->  18x18x128  input padding 0  output padding 1<br>Convolution(ReLU) :  18x18x128  ->  16x16x256  input padding 1  output padding 0<br>MaxPooling :  16x16x256  ->  8x8x256  input padding 0  output padding 0<br>BatchNormalization :  8x8x256  ->  10x10x256  input padding 0  output padding 1<br>Convolution(ReLU) :  10x10x256  ->  8x8x512  input padding 1  output padding 0<br>MaxPooling :  8x8x512  ->  4x4x512  input padding 0  output padding 0<br>BatchNormalization :  4x4x512  ->  6x6x512  input padding 0  output padding 1<br>Convolution(ReLU) :  6x6x512  ->  4x4x1024  input padding 1  output padding 0<br>BatchNormalization :  4x4x1024  ->  4x4x1024  input padding 0  output padding 0<br>Convolution(ReLU) :  4x4x1024  ->  4x4x1000  input padding 0  output padding 0<br>AveragePooling :  4x4x1000  ->  1x1x1000  input padding 0  output padding 0<br>Softmax :  1x1x1000  ->  1x1x1  input padding 0  output padding 0<br>
|-
| Output | [ILSVRC2012 1000 classes](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/ILSVRC2012_labels.txt)
|-
| Notes | Trained by Chuck Jacobs using CNTK 2.1
|=
