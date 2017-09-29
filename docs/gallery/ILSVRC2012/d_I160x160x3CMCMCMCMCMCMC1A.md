---
layout: default
title: 160x160x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/d_I160x160x3CMCMCMCMCMCMC1A
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 160x160x3 Convolutional Neural Network (59.01% top 1 accuracy, 81.74% top 5 accuracy, 0.30s/frame on Raspberry Pi 3)

<table>
    <tr>
        <td> Download </td>
        <td colspan="3"> <a href="https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I160x160x3CMCMCMCMCMCMC1A/d_I160x160x3CMCMCMCMCMCMC1A.ell.zip">d_I160x160x3CMCMCMCMCMCMC1A.ell.zip</a></td>
    </tr>
    <tr>
        <td> Accuracy </td>
        <td colspan="3"> ILSVRC2012: 81.74% (Top 5), 59.01% (Top 1) </td>
    </tr>
    <tr>
        <td> Performance </td>
        <td colspan="3"> Raspberry Pi 3 (Raspbian) @ 700MHz: 0.30s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 600MHz: 0.33s/frame<br>DragonBoard 410c @ 1.2GHz: 0.18s/frame </td>
    </tr>
    <tr>
        <td> Uncompressed Size </td>
        <td colspan="3"> 91MB </td>
    </tr>
    <tr>
        <td> Input </td>
        <td colspan="3"> 160 x 160 x {B,G,R} </td>
    </tr>
    <tr>
        <td rowspan="17"> Architecture </td>
        <tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 160x160x16</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 80x80x16</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 80x80x64</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 40x40x64</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 40x40x64</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 20x20x64</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 20x20x128</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 10x10x128</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 10x10x256</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 5x5x256</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 5x5x512</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 3x3x512</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 3x3x1024</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 3x3x1000</td>
	<td>size=1x1, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 1x1x1000</td>
	<td>size=3x3, stride=1, operation=average</td>
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

