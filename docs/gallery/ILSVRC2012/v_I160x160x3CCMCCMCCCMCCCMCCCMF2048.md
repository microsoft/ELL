---
layout: default
title: 160x160x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/v_I160x160x3CCMCCMCCCMCCCMCCCMF2048
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 160x160x3 Convolutional Neural Network (68.39% top 1 accuracy, 88.38% top 5 accuracy, 4.72s/frame on Raspberry Pi 3)

<table>
    <tr>
        <td> Download </td>
        <td colspan="3"> <a href="https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/v_I160x160x3CCMCCMCCCMCCCMCCCMF2048/v_I160x160x3CCMCCMCCCMCCCMCCCMF2048.ell.zip">v_I160x160x3CCMCCMCCCMCCCMCCCMF2048.ell.zip</a></td>
    </tr>
    <tr>
        <td> Accuracy </td>
        <td colspan="3"> ILSVRC2012: 88.38% (Top 5), 68.39% (Top 1) </td>
    </tr>
    <tr>
        <td> Performance </td>
        <td colspan="3"> Raspberry Pi 3 (Raspbian) @ 700MHz: 4.72s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 600MHz: 4.29s/frame<br>DragonBoard 410c @ 1.2GHz: 2.98s/frame </td>
    </tr>
    <tr>
        <td> Uncompressed Size </td>
        <td colspan="3"> 535MB </td>
    </tr>
    <tr>
        <td> Input </td>
        <td colspan="3"> 160 x 160 x {B,G,R} </td>
    </tr>
    <tr>
        <td rowspan="20"> Architecture </td>
        <tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 160x160x64</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 160x160x64</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 82x82x64</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 80x80x128</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 80x80x128</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 42x42x128</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 40x40x256</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 40x40x256</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 40x40x256</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 22x22x256</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 20x20x512</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 20x20x512</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 20x20x512</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 12x12x512</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 10x10x512</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 10x10x512</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 10x10x512</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 5x5x512</td>
	<td>size=2x2, stride=2, operation=max</td>
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

