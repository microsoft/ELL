---
layout: default
title: binary
permalink: /gallery/VGG-16
---

[Back to Gallery](/ELL/gallery)

## Imagenet Classification: 64x64 Binarized Convolutional Neural Network

|=
| Download | [v_I[64x64x3]BBMBBMBBBMBBBMBBCMF[4096]F[4096].ell.zip](https://github.com/Microsoft/ELL-models/raw/master/models/v_I%5B64x64x3%5DBBMBBMBBBMBBBMBBCMF%5B4096%5DF%5B4096%5D/v_I%5B64x64x3%5DBBMBBMBBBMBBBMBBCMF%5B4096%5DF%5B4096%5D.model) 
|-
| Accuracy | ImageNet: 0.9533<br>Some-other-benchmark: 0.733
|-
| Performance | Raspberry Pi 3 (Raspian) @ 700MHz: 100ms/frame<br>Raspberry Pi 3 (OpenSUSE) @ 800MHz: 40ms/frame<br>DragonBoard 410c @ 1.2GHz: 20ms/frame
|-
| Uncompressed Size | 107MB
|-
| Input | 64 x 64 x {R,G,B}
|-
| Architecure | Binarized Convolutional Layer (filters=32, filter-size=3x3, stride=1)<br>Binarized Convolutional Layer (filters=32, filter-size=3x3, stride=1)<br>Max Pooling Layer<br>Binarized Convolutional Layer<br>Binarized Convolutional Layer<br>Max Pooling Layer<br>Binarized Convolutional Layer<br>Binarized Convolutional Layer<br>Binarized Convolutional Layer<br>Max Pooling Layer<br>Binarized Convolutional Layer<br>Binarized Convolutional Layer<br>Binarized Convolutional Layer<br>Max Pooling Layer<br>Binarized Convolutional Layer<br>Binarized Convolutional Layer<br>Convolutional Layer<br>Max Pooling Layer<br>Fully Connected Layer<br>Fully Connected Layer
|-
| Output | 1000 ImageNet classes
|-
| Notes | Trained by Chuck Jacobs using CNTK
|=



