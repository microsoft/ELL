---
layout: default
title: 192x192x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/v_I192x192x3CCMCCMCCCMCCCMCCCMF1024F1024
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 192x192x3 Convolutional Neural Network (63.50% top 1 accuracy, 85.33% top 5 accuracy, 7.42s/frame on Raspberry Pi 3)

|=
| Download | [v_I192x192x3BBMBBMBBBMBBBMBBBMF1024F1024.ell.zip](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/v_I192x192x3BBMBBMBBBMBBBMBBBMF1024F1024/v_I192x192x3BBMBBMBBBMBBBMBBBMF1024F1024.ell.zip)
|-
| Accuracy | ILSVRC2012: 85.33% (Top 5), 63.50% (Top 1) 
|-
| Performance | Raspberry Pi 3 (Raspbian) @ 700MHz: 7.42s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 800MHz: @pi3_64_SECONDS_PER_FRAME<br>DragonBoard 410c @ 1.2GHz: @aarch64_SECONDS_PER_FRAME@s/frame
|-
| Uncompressed Size | 443MB
|-
| Input | 192 x 192 x {R,G,B}
|-
| Architecure | Minus :  192x192x3  ->  194x194x3  input padding 0  output padding 1<br>Convolution(ReLU) :  194x194x3  ->  194x194x64  input padding 1  output padding 1<br>Convolution(ReLU) :  194x194x64  ->  192x192x64  input padding 1  output padding 0<br>MaxPooling :  192x192x64  ->  98x98x64  input padding 0  output padding 1<br>Convolution(ReLU) :  98x98x64  ->  98x98x128  input padding 1  output padding 1<br>Convolution(ReLU) :  98x98x128  ->  96x96x128  input padding 1  output padding 0<br>MaxPooling :  96x96x128  ->  50x50x128  input padding 0  output padding 1<br>Convolution(ReLU) :  50x50x128  ->  50x50x256  input padding 1  output padding 1<br>Convolution(ReLU) :  50x50x256  ->  50x50x256  input padding 1  output padding 1<br>Convolution(ReLU) :  50x50x256  ->  48x48x256  input padding 1  output padding 0<br>MaxPooling :  48x48x256  ->  26x26x256  input padding 0  output padding 1<br>Convolution(ReLU) :  26x26x256  ->  26x26x512  input padding 1  output padding 1<br>Convolution(ReLU) :  26x26x512  ->  26x26x512  input padding 1  output padding 1<br>Convolution(ReLU) :  26x26x512  ->  24x24x512  input padding 1  output padding 0<br>MaxPooling :  24x24x512  ->  14x14x512  input padding 0  output padding 1<br>Convolution(ReLU) :  14x14x512  ->  14x14x512  input padding 1  output padding 1<br>Convolution(ReLU) :  14x14x512  ->  14x14x512  input padding 1  output padding 1<br>Convolution(ReLU) :  14x14x512  ->  12x12x512  input padding 1  output padding 0<br>MaxPooling :  12x12x512  ->  6x6x512  input padding 0  output padding 0<br>Dense :  6x6x512  ->  1x1x1024  input padding 0  output padding 0<br>Dense :  1x1x1024  ->  1x1x1024  input padding 0  output padding 0<br>Dense :  1x1x1024  ->  1x1x1000  input padding 0  output padding 0<br>ElementTimes :  1x1x1000  ->  1x1x1000  input padding 0  output padding 0<br>Softmax :  1x1x1000  ->  1x1x1  input padding 0  output padding 0<br>
|-
| Output | [ILSVRC2012 1000 classes](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/ILSVRC2012_labels.txt)
|-
| Notes | Trained by Chuck Jacobs using CNTK 2.1
|=
