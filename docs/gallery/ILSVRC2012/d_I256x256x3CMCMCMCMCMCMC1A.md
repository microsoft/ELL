---
layout: default
title: 256x256x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/d_I256x256x3CMCMCMCMCMCMC1A
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 256x256x3 Convolutional Neural Network (57.60% top 1 accuracy, 80.39% top 5 accuracy, 0.68s/frame on Raspberry Pi 3)

<table>
    <tr>
        <td> Download </td>
        <td colspan="3"> <a href="https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I256x256x3CMCMCMCMCMCMC1A/d_I256x256x3CMCMCMCMCMCMC1A.ell.zip">d_I256x256x3CMCMCMCMCMCMC1A.ell.zip</a></td>
    </tr>
    <tr>
        <td> Accuracy </td>
        <td colspan="3"> ILSVRC2012: 80.39% (Top 5), 57.60% (Top 1) </td>
    </tr>
    <tr>
        <td> Performance </td>
        <td colspan="3"> Raspberry Pi 3 (Raspbian) @ 700MHz: 0.68s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 600MHz: 0.88s/frame<br>DragonBoard 410c @ 1.2GHz: 0.54s/frame </td>
    </tr>
    <tr>
        <td> Uncompressed Size </td>
        <td colspan="3"> 91MB </td>
    </tr>
    <tr>
        <td> Input </td>
        <td colspan="3"> 256 x 256 x {B,G,R} </td>
    </tr>
    <tr>
        <td rowspan="17"> Architecture </td>
        <tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 256x256x16</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 128x128x16</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 128x128x64</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 64x64x64</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 64x64x64</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 32x32x64</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 32x32x128</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 16x16x128</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 16x16x256</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 8x8x256</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 8x8x512</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 4x4x512</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 4x4x1024</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 4x4x1000</td>
	<td>size=1x1, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 1x1x1000</td>
	<td>size=4x4, stride=1, operation=average</td>
</tr>
<tr class="table-row-condensed">
	<td>Softmax</td>
	<td>&#8680; 1x1x1000</td>
	<td></td>
</tr>

    </tr>
    <tr>
        <td> Output </td>
        <td colspan="3"> <a href="https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/categories.txt">ILSVRC2012 1000 classes</a> </td>
    </tr>
    <tr>
        <td> Notes </td>
        <td colspan="3"> Trained by Chuck Jacobs using CNTK 2.1 </td>
    </tr>
</table>

