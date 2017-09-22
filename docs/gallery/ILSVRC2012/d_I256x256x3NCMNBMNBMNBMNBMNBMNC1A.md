---
layout: default
title: 256x256x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/d_I256x256x3NCMNBMNBMNBMNBMNBMNC1A
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 256x256x3 Convolutional Neural Network (38.26% top 1 accuracy, 62.46% top 5 accuracy, 1.71s/frame on Raspberry Pi 3)

|=
| Download | [d_I256x256x3NCMNBMNBMNBMNBMNBMNC1A.ell.zip](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I256x256x3NCMNBMNBMNBMNBMNBMNC1A/d_I256x256x3NCMNBMNBMNBMNBMNBMNC1A.ell.zip)
|-
| Accuracy | ILSVRC2012: 62.46% (Top 5), 38.26% (Top 1) 
|-
| Performance | Raspberry Pi 3 (Raspbian) @ 700MHz: 1.71s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 600MHz: 1.36s/frame<br>DragonBoard 410c @ 1.2GHz: 0.78s/frame
|-
| Uncompressed Size | 35MB
|-
| Input | 256 x 256 x {R,G,B}
|-
| Architecure | Minus :  256x256x3  ->  258x258x3  input padding 0  output padding 1<br>Convolution(ReLU) :  258x258x3  ->  256x256x16  input padding 1  output padding 0<br>BatchNormalization :  256x256x16  ->  256x256x16  input padding 0  output padding 0<br>MaxPooling :  256x256x16  ->  130x130x16  input padding 0  output padding 1<br>BinaryConvolution :  130x130x16  ->  128x128x64  input padding 1  output padding 0<br>Plus :  128x128x64  ->  128x128x64  input padding 0  output padding 0<br>PReLU :  128x128x64  ->  130x130x64  input padding 0  output padding 1<br>MaxPooling :  130x130x64  ->  64x64x64  input padding 1  output padding 0<br>BatchNormalization :  64x64x64  ->  66x66x64  input padding 0  output padding 1<br>BinaryConvolution :  66x66x64  ->  64x64x64  input padding 1  output padding 0<br>Plus :  64x64x64  ->  64x64x64  input padding 0  output padding 0<br>PReLU :  64x64x64  ->  66x66x64  input padding 0  output padding 1<br>MaxPooling :  66x66x64  ->  32x32x64  input padding 1  output padding 0<br>BatchNormalization :  32x32x64  ->  34x34x64  input padding 0  output padding 1<br>BinaryConvolution :  34x34x64  ->  32x32x128  input padding 1  output padding 0<br>Plus :  32x32x128  ->  32x32x128  input padding 0  output padding 0<br>PReLU :  32x32x128  ->  34x34x128  input padding 0  output padding 1<br>MaxPooling :  34x34x128  ->  16x16x128  input padding 1  output padding 0<br>BatchNormalization :  16x16x128  ->  18x18x128  input padding 0  output padding 1<br>BinaryConvolution :  18x18x128  ->  16x16x256  input padding 1  output padding 0<br>Plus :  16x16x256  ->  16x16x256  input padding 0  output padding 0<br>PReLU :  16x16x256  ->  18x18x256  input padding 0  output padding 1<br>MaxPooling :  18x18x256  ->  8x8x256  input padding 1  output padding 0<br>BatchNormalization :  8x8x256  ->  10x10x256  input padding 0  output padding 1<br>BinaryConvolution :  10x10x256  ->  8x8x512  input padding 1  output padding 0<br>Plus :  8x8x512  ->  8x8x512  input padding 0  output padding 0<br>PReLU :  8x8x512  ->  10x10x512  input padding 0  output padding 1<br>MaxPooling :  10x10x512  ->  4x4x512  input padding 1  output padding 0<br>BatchNormalization :  4x4x512  ->  6x6x512  input padding 0  output padding 1<br>BinaryConvolution :  6x6x512  ->  4x4x1024  input padding 1  output padding 0<br>Plus :  4x4x1024  ->  4x4x1024  input padding 0  output padding 0<br>PReLU :  4x4x1024  ->  6x6x1024  input padding 0  output padding 1<br>MaxPooling :  6x6x1024  ->  2x2x1024  input padding 1  output padding 0<br>BatchNormalization :  2x2x1024  ->  2x2x1024  input padding 0  output padding 0<br>Convolution :  2x2x1024  ->  2x2x1000  input padding 0  output padding 0<br>AveragePooling :  2x2x1000  ->  1x1x1000  input padding 0  output padding 0<br>ElementTimes :  1x1x1000  ->  1x1x1000  input padding 0  output padding 0<br>Softmax :  1x1x1000  ->  1x1x1  input padding 0  output padding 0<br>
|-
| Output | [ILSVRC2012 1000 classes](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/ILSVRC2012_labels.txt)
|-
| Notes | Trained by Chuck Jacobs using CNTK 2.1
|=
