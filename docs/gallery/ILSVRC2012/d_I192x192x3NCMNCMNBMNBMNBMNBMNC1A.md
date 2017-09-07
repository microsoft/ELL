---
layout: default
title: 192x192x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/d_I192x192x3NCMNCMNBMNBMNBMNBMNC1A
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 192x192x3 Convolutional Neural Network (62.78% top 1 error, 37.35% top 5 error, 1.08s/frame)

|=
| Download | [d_I192x192x3NCMNCMNBMNBMNBMNBMNC1A.ell.zip](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I192x192x3NCMNCMNBMNBMNBMNBMNC1A/d_I192x192x3NCMNCMNBMNBMNBMNBMNC1A.ell.zip)
|-
| Accuracy | ILSVRC2012: 62.65% (Top 5), 37.22% (Top 1) 
|-
| Performance | Raspberry Pi 3 (Raspbian) @ 700MHz: 1.08s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 800MHz: <br>DragonBoard 410c @ 1.2GHz:
|-
| Uncompressed Size | 18MB
|-
| Input | 192 x 192 x {R,G,B}
|-
| Architecure | Minus :  192x192x3  ->  194x194x3  input padding 0  output padding 1<br>Convolution(ReLU) :  194x194x3  ->  192x192x16  input padding 1  output padding 0<br>BatchNormalization :  192x192x16  ->  192x192x16  input padding 0  output padding 0<br>MaxPooling :  192x192x16  ->  98x98x16  input padding 0  output padding 1<br>Convolution(ReLU) :  98x98x16  ->  96x96x64  input padding 1  output padding 0<br>BatchNormalization :  96x96x64  ->  96x96x64  input padding 0  output padding 0<br>MaxPooling :  96x96x64  ->  50x50x64  input padding 0  output padding 1<br>BinaryConvolution :  50x50x64  ->  48x48x64  input padding 1  output padding 0<br>Plus :  48x48x64  ->  48x48x64  input padding 0  output padding 0<br>PReLU :  48x48x64  ->  50x50x64  input padding 0  output padding 1<br>MaxPooling :  50x50x64  ->  24x24x64  input padding 1  output padding 0<br>BatchNormalization :  24x24x64  ->  26x26x64  input padding 0  output padding 1<br>BinaryConvolution :  26x26x64  ->  24x24x128  input padding 1  output padding 0<br>Plus :  24x24x128  ->  24x24x128  input padding 0  output padding 0<br>PReLU :  24x24x128  ->  26x26x128  input padding 0  output padding 1<br>MaxPooling :  26x26x128  ->  12x12x128  input padding 1  output padding 0<br>BatchNormalization :  12x12x128  ->  14x14x128  input padding 0  output padding 1<br>BinaryConvolution :  14x14x128  ->  12x12x256  input padding 1  output padding 0<br>Plus :  12x12x256  ->  12x12x256  input padding 0  output padding 0<br>PReLU :  12x12x256  ->  14x14x256  input padding 0  output padding 1<br>MaxPooling :  14x14x256  ->  6x6x256  input padding 1  output padding 0<br>BatchNormalization :  6x6x256  ->  8x8x256  input padding 0  output padding 1<br>BinaryConvolution :  8x8x256  ->  6x6x512  input padding 1  output padding 0<br>Plus :  6x6x512  ->  6x6x512  input padding 0  output padding 0<br>PReLU :  6x6x512  ->  8x8x512  input padding 0  output padding 1<br>MaxPooling :  8x8x512  ->  3x3x512  input padding 1  output padding 0<br>BatchNormalization :  3x3x512  ->  5x5x512  input padding 0  output padding 1<br>BinaryConvolution :  5x5x512  ->  3x3x1024  input padding 1  output padding 0<br>Plus :  3x3x1024  ->  3x3x1024  input padding 0  output padding 0<br>PReLU :  3x3x1024  ->  5x5x1024  input padding 0  output padding 1<br>MaxPooling :  5x5x1024  ->  2x2x1024  input padding 1  output padding 0<br>BatchNormalization :  2x2x1024  ->  2x2x1024  input padding 0  output padding 0<br>Convolution :  2x2x1024  ->  2x2x1000  input padding 0  output padding 0<br>AveragePooling :  2x2x1000  ->  1x1x1000  input padding 0  output padding 0<br>ElementTimes :  1x1x1000  ->  1x1x1000  input padding 0  output padding 0<br>Softmax :  1x1x1000  ->  1x1x1  input padding 0  output padding 0<br>
|-
| Output | [ILSVRC2012 1000 classes](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/ImageNetLabels.txt)
|-
| Notes | Trained by Chuck Jacobs using CNTK 2.1
|=
