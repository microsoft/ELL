---
layout: default
title: 160x160x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/v_I160x160x3CCMCCMCCCMCCCMCCCMF2048
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 160x160x3 Convolutional Neural Network (68.39% top 1 accuracy, 88.38% top 5 accuracy, 5.15s/frame on Raspberry Pi 3)

|=
| Download | [v_I160x160x3BBMBBMBBBMBBBMBBBMF2048.ell.zip](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/v_I160x160x3BBMBBMBBBMBBBMBBBMF2048/v_I160x160x3BBMBBMBBBMBBBMBBBMF2048.ell.zip)
|-
| Accuracy | ILSVRC2012: 88.38% (Top 5), 68.39% (Top 1) 
|-
| Performance | Raspberry Pi 3 (Raspbian) @ 700MHz: 5.15s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 800MHz: @pi3_64_SECONDS_PER_FRAME<br>DragonBoard 410c @ 1.2GHz: 3.54s/frame
|-
| Uncompressed Size | 535MB
|-
| Input | 160 x 160 x {R,G,B}
|-
| Architecure | Minus :  160x160x3  ->  162x162x3  input padding 0  output padding 1<br>Convolution(ReLU) :  162x162x3  ->  162x162x64  input padding 1  output padding 1<br>Convolution(ReLU) :  162x162x64  ->  160x160x64  input padding 1  output padding 0<br>MaxPooling :  160x160x64  ->  82x82x64  input padding 0  output padding 1<br>Convolution(ReLU) :  82x82x64  ->  82x82x128  input padding 1  output padding 1<br>Convolution(ReLU) :  82x82x128  ->  80x80x128  input padding 1  output padding 0<br>MaxPooling :  80x80x128  ->  42x42x128  input padding 0  output padding 1<br>Convolution(ReLU) :  42x42x128  ->  42x42x256  input padding 1  output padding 1<br>Convolution(ReLU) :  42x42x256  ->  42x42x256  input padding 1  output padding 1<br>Convolution(ReLU) :  42x42x256  ->  40x40x256  input padding 1  output padding 0<br>MaxPooling :  40x40x256  ->  22x22x256  input padding 0  output padding 1<br>Convolution(ReLU) :  22x22x256  ->  22x22x512  input padding 1  output padding 1<br>Convolution(ReLU) :  22x22x512  ->  22x22x512  input padding 1  output padding 1<br>Convolution(ReLU) :  22x22x512  ->  20x20x512  input padding 1  output padding 0<br>MaxPooling :  20x20x512  ->  12x12x512  input padding 0  output padding 1<br>Convolution(ReLU) :  12x12x512  ->  12x12x512  input padding 1  output padding 1<br>Convolution(ReLU) :  12x12x512  ->  12x12x512  input padding 1  output padding 1<br>Convolution(ReLU) :  12x12x512  ->  10x10x512  input padding 1  output padding 0<br>MaxPooling :  10x10x512  ->  5x5x512  input padding 0  output padding 0<br>Dense :  5x5x512  ->  1x1x2048  input padding 0  output padding 0<br>Dense :  1x1x2048  ->  1x1x1000  input padding 0  output padding 0<br>ElementTimes :  1x1x1000  ->  1x1x1000  input padding 0  output padding 0<br>Softmax :  1x1x1000  ->  1x1x1  input padding 0  output padding 0<br>
|-
| Output | [ILSVRC2012 1000 classes](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/ILSVRC2012_labels.txt)
|-
| Notes | Trained by Chuck Jacobs using CNTK 2.1
|=
