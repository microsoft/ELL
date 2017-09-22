---
layout: default
title: 64x64x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/v_I64x64x3CCMCCMCCCMCCCMCCCMF2048
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 64x64x3 Convolutional Neural Network (57.12% top 1 accuracy, 80.49% top 5 accuracy, 0.82s/frame on Raspberry Pi 3)

|=
| Download | [v_I64x64x3CCMCCMCCCMCCCMCCCMF2048.ell.zip](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/v_I64x64x3CCMCCMCCCMCCCMCCCMF2048/v_I64x64x3CCMCCMCCCMCCCMCCCMF2048.ell.zip)
|-
| Accuracy | ILSVRC2012: 80.49% (Top 5), 57.12% (Top 1) 
|-
| Performance | Raspberry Pi 3 (Raspbian) @ 700MHz: 0.82s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 600MHz: 0.84s/frame<br>DragonBoard 410c @ 1.2GHz: 0.44s/frame
|-
| Uncompressed Size | 257MB
|-
| Input | 64 x 64 x {R,G,B}
|-
| Architecure | Minus :  64x64x3  ->  66x66x3  input padding 0  output padding 1<br>Convolution(ReLU) :  66x66x3  ->  66x66x64  input padding 1  output padding 1<br>Convolution(ReLU) :  66x66x64  ->  64x64x64  input padding 1  output padding 0<br>MaxPooling :  64x64x64  ->  34x34x64  input padding 0  output padding 1<br>Convolution(ReLU) :  34x34x64  ->  34x34x128  input padding 1  output padding 1<br>Convolution(ReLU) :  34x34x128  ->  32x32x128  input padding 1  output padding 0<br>MaxPooling :  32x32x128  ->  18x18x128  input padding 0  output padding 1<br>Convolution(ReLU) :  18x18x128  ->  18x18x256  input padding 1  output padding 1<br>Convolution(ReLU) :  18x18x256  ->  18x18x256  input padding 1  output padding 1<br>Convolution(ReLU) :  18x18x256  ->  16x16x256  input padding 1  output padding 0<br>MaxPooling :  16x16x256  ->  10x10x256  input padding 0  output padding 1<br>Convolution(ReLU) :  10x10x256  ->  10x10x512  input padding 1  output padding 1<br>Convolution(ReLU) :  10x10x512  ->  10x10x512  input padding 1  output padding 1<br>Convolution(ReLU) :  10x10x512  ->  8x8x512  input padding 1  output padding 0<br>MaxPooling :  8x8x512  ->  6x6x512  input padding 0  output padding 1<br>Convolution(ReLU) :  6x6x512  ->  6x6x512  input padding 1  output padding 1<br>Convolution(ReLU) :  6x6x512  ->  6x6x512  input padding 1  output padding 1<br>Convolution(ReLU) :  6x6x512  ->  4x4x512  input padding 1  output padding 0<br>MaxPooling :  4x4x512  ->  2x2x512  input padding 0  output padding 0<br>Dense :  2x2x512  ->  1x1x2048  input padding 0  output padding 0<br>Dense :  1x1x2048  ->  1x1x1000  input padding 0  output padding 0<br>ElementTimes :  1x1x1000  ->  1x1x1000  input padding 0  output padding 0<br>Softmax :  1x1x1000  ->  1x1x1  input padding 0  output padding 0<br>
|-
| Output | [ILSVRC2012 1000 classes](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/ILSVRC2012_labels.txt)
|-
| Notes | Trained by Chuck Jacobs using CNTK 2.1
|=
