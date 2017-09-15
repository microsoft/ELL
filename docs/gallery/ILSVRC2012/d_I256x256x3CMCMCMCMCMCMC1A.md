---
layout: default
title: 256x256x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/d_I256x256x3CMCMCMCMCMCMC1A
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 256x256x3 Convolutional Neural Network (60.84% top 1 accuracy, 83.03% top 5 accuracy, 1.45s/frame on Raspberry Pi 3)

|=
| Download | [d_I256x256x3CMCMCMCMCMCMC1A.ell.zip](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I256x256x3CMCMCMCMCMCMC1A/d_I256x256x3CMCMCMCMCMCMC1A.ell.zip)
|-
| Accuracy | ILSVRC2012: 83.03% (Top 5), 60.84% (Top 1) 
|-
| Performance | Raspberry Pi 3 (Raspbian) @ 700MHz: 1.45s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 800MHz: <br>DragonBoard 410c @ 1.2GHz:
|-
| Uncompressed Size | 91MB
|-
| Input | 256 x 256 x {R,G,B}
|-
| Architecure | Minus :  256x256x3  ->  258x258x3  input padding 0<br>Convolution(LeakyReLU) :  258x258x3  ->  256x256x16  input padding 1<br>BatchNormalization :  256x256x16  ->  256x256x16  input padding 0<br>MaxPooling :  256x256x16  ->  130x130x16  input padding 0<br>Convolution(LeakyReLU) :  130x130x16  ->  128x128x64  input padding 1<br>BatchNormalization :  128x128x64  ->  128x128x64  input padding 0<br>MaxPooling :  128x128x64  ->  66x66x64  input padding 0<br>Convolution(LeakyReLU) :  66x66x64  ->  64x64x64  input padding 1<br>BatchNormalization :  64x64x64  ->  64x64x64  input padding 0<br>MaxPooling :  64x64x64  ->  34x34x64  input padding 0<br>Convolution(LeakyReLU) :  34x34x64  ->  32x32x128  input padding 1<br>BatchNormalization :  32x32x128  ->  32x32x128  input padding 0<br>MaxPooling :  32x32x128  ->  18x18x128  input padding 0<br>Convolution(LeakyReLU) :  18x18x128  ->  16x16x256  input padding 1<br>BatchNormalization :  16x16x256  ->  16x16x256  input padding 0<br>MaxPooling :  16x16x256  ->  10x10x256  input padding 0<br>Convolution(LeakyReLU) :  10x10x256  ->  8x8x512  input padding 1<br>BatchNormalization :  8x8x512  ->  8x8x512  input padding 0<br>MaxPooling :  8x8x512  ->  6x6x512  input padding 0<br>Convolution(LeakyReLU) :  6x6x512  ->  4x4x1024  input padding 1<br>BatchNormalization :  4x4x1024  ->  4x4x1024  input padding 0<br>Convolution :  4x4x1024  ->  4x4x1000  input padding 0<br>AveragePooling :  4x4x1000  ->  1x1x1000  input padding 0<br>Softmax :  1x1x1000  ->  1x1x1  input padding 0<br>
|-
| Output | [ILSVRC2012 1000 classes](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/ILSVRC2012_labels.txt)
|-
| Notes | Trained by Chuck Jacobs using CNTK 2.1
|=
