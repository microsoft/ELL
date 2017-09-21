---
layout: default
title: 160x160x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/d_I160x160x3NCMNCMNBMNBMNBMNBMNB1A
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 160x160x3 Convolutional Neural Network (38.08% top 1 accuracy, 63.16% top 5 accuracy, 0.66s/frame on Raspberry Pi 3)

|=
| Download | [d_I160x160x3CMCMNBMNBMNBMNBMNB1A.ell.zip](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I160x160x3CMCMNBMNBMNBMNBMNB1A/d_I160x160x3CMCMNBMNBMNBMNBMNB1A.ell.zip)
|-
| Accuracy | ILSVRC2012: 63.16% (Top 5), 38.08% (Top 1) 
|-
| Performance | Raspberry Pi 3 (Raspbian) @ 700MHz: 0.66s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 600MHz: 0.50s/frame<br>DragonBoard 410c @ 1.2GHz: 0.29s/frame
|-
| Uncompressed Size | 17MB
|-
| Input | 160 x 160 x {R,G,B}
|-
| Architecure | Minus :  160x160x3  ->  162x162x3  input padding 0  output padding 1<br>Convolution(ReLU) :  162x162x3  ->  160x160x16  input padding 1  output padding 0<br>BatchNormalization :  160x160x16  ->  160x160x16  input padding 0  output padding 0<br>MaxPooling :  160x160x16  ->  82x82x16  input padding 0  output padding 1<br>Convolution(ReLU) :  82x82x16  ->  80x80x64  input padding 1  output padding 0<br>BatchNormalization :  80x80x64  ->  80x80x64  input padding 0  output padding 0<br>MaxPooling :  80x80x64  ->  42x42x64  input padding 0  output padding 1<br>BinaryConvolution :  42x42x64  ->  40x40x64  input padding 1  output padding 0<br>Plus :  40x40x64  ->  40x40x64  input padding 0  output padding 0<br>PReLU :  40x40x64  ->  42x42x64  input padding 0  output padding 1<br>MaxPooling :  42x42x64  ->  20x20x64  input padding 1  output padding 0<br>BatchNormalization :  20x20x64  ->  22x22x64  input padding 0  output padding 1<br>BinaryConvolution :  22x22x64  ->  20x20x128  input padding 1  output padding 0<br>Plus :  20x20x128  ->  20x20x128  input padding 0  output padding 0<br>PReLU :  20x20x128  ->  22x22x128  input padding 0  output padding 1<br>MaxPooling :  22x22x128  ->  10x10x128  input padding 1  output padding 0<br>BatchNormalization :  10x10x128  ->  12x12x128  input padding 0  output padding 1<br>BinaryConvolution :  12x12x128  ->  10x10x256  input padding 1  output padding 0<br>Plus :  10x10x256  ->  10x10x256  input padding 0  output padding 0<br>PReLU :  10x10x256  ->  12x12x256  input padding 0  output padding 1<br>MaxPooling :  12x12x256  ->  5x5x256  input padding 1  output padding 0<br>BatchNormalization :  5x5x256  ->  7x7x256  input padding 0  output padding 1<br>BinaryConvolution :  7x7x256  ->  5x5x512  input padding 1  output padding 0<br>Plus :  5x5x512  ->  5x5x512  input padding 0  output padding 0<br>PReLU :  5x5x512  ->  7x7x512  input padding 0  output padding 1<br>MaxPooling :  7x7x512  ->  3x3x512  input padding 1  output padding 0<br>BatchNormalization :  3x3x512  ->  5x5x512  input padding 0  output padding 1<br>BinaryConvolution :  5x5x512  ->  3x3x1024  input padding 1  output padding 0<br>Plus :  3x3x1024  ->  3x3x1024  input padding 0  output padding 0<br>PReLU :  3x3x1024  ->  5x5x1024  input padding 0  output padding 1<br>MaxPooling :  5x5x1024  ->  2x2x1024  input padding 1  output padding 0<br>BatchNormalization :  2x2x1024  ->  2x2x1024  input padding 0  output padding 0<br>Convolution :  2x2x1024  ->  2x2x1000  input padding 0  output padding 0<br>AveragePooling :  2x2x1000  ->  1x1x1000  input padding 0  output padding 0<br>ElementTimes :  1x1x1000  ->  1x1x1000  input padding 0  output padding 0<br>Softmax :  1x1x1000  ->  1x1x1  input padding 0  output padding 0<br>
|-
| Output | [ILSVRC2012 1000 classes](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/ILSVRC2012_labels.txt)
|-
| Notes | Trained by Chuck Jacobs using CNTK 2.1
|=
