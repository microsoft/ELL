---
layout: default
title: 224x224x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/d_I224x224x3CMCMCMCMCMCMC1A
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 224x224x3 Convolutional Neural Network (60.03% top 1 accuracy, 82.49% top 5 accuracy, 0.53s/frame on Raspberry Pi 3)

|=
| Download | [d_I224x224x3CMCMCMCMCMCMC1A.ell.zip](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3CMCMCMCMCMCMC1A/d_I224x224x3CMCMCMCMCMCMC1A.ell.zip)
|-
| Accuracy | ILSVRC2012: 82.49% (Top 5), 60.03% (Top 1) 
|-
| Performance | Raspberry Pi 3 (Raspbian) @ 700MHz: 0.53s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 600MHz: 0.52s/frame<br>DragonBoard 410c @ 1.2GHz: 0.29s/frame
|-
| Uncompressed Size | 91MB
|-
| Input | 224 x 224 x {R,G,B}
|-
| Architecure | Minus :  224x224x3  ->  226x226x3  input padding 0  output padding 1<br>Convolution(LeakyReLU) :  226x226x3  ->  224x224x16  input padding 1  output padding 0<br>BatchNormalization :  224x224x16  ->  224x224x16  input padding 0  output padding 0<br>MaxPooling :  224x224x16  ->  114x114x16  input padding 0  output padding 1<br>Convolution(LeakyReLU) :  114x114x16  ->  112x112x64  input padding 1  output padding 0<br>BatchNormalization :  112x112x64  ->  112x112x64  input padding 0  output padding 0<br>MaxPooling :  112x112x64  ->  58x58x64  input padding 0  output padding 1<br>Convolution(LeakyReLU) :  58x58x64  ->  56x56x64  input padding 1  output padding 0<br>BatchNormalization :  56x56x64  ->  56x56x64  input padding 0  output padding 0<br>MaxPooling :  56x56x64  ->  30x30x64  input padding 0  output padding 1<br>Convolution(LeakyReLU) :  30x30x64  ->  28x28x128  input padding 1  output padding 0<br>BatchNormalization :  28x28x128  ->  28x28x128  input padding 0  output padding 0<br>MaxPooling :  28x28x128  ->  16x16x128  input padding 0  output padding 1<br>Convolution(LeakyReLU) :  16x16x128  ->  14x14x256  input padding 1  output padding 0<br>BatchNormalization :  14x14x256  ->  14x14x256  input padding 0  output padding 0<br>MaxPooling :  14x14x256  ->  9x9x256  input padding 0  output padding 1<br>Convolution(LeakyReLU) :  9x9x256  ->  7x7x512  input padding 1  output padding 0<br>BatchNormalization :  7x7x512  ->  7x7x512  input padding 0  output padding 0<br>MaxPooling :  7x7x512  ->  6x6x512  input padding 0  output padding 1<br>Convolution(LeakyReLU) :  6x6x512  ->  4x4x1024  input padding 1  output padding 0<br>BatchNormalization :  4x4x1024  ->  4x4x1024  input padding 0  output padding 0<br>Convolution :  4x4x1024  ->  4x4x1000  input padding 0  output padding 0<br>AveragePooling :  4x4x1000  ->  1x1x1000  input padding 0  output padding 0<br>Softmax :  1x1x1000  ->  1x1x1  input padding 0  output padding 0<br>
|-
| Output | [ILSVRC2012 1000 classes](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/categories.txt)
|-
| Notes | Trained by Chuck Jacobs using CNTK 2.1
|=
