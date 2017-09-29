---
layout: default
title: 224x224x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/d_I224x224x3NCMNCMNBMNBMNBMNBMNB1A
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 224x224x3 Convolutional Neural Network (37.22% top 1 accuracy, 62.63% top 5 accuracy, 0.52s/frame on Raspberry Pi 3)

<table>
    <tr>
        <td> Download </td>
        <td colspan="3"> <a href="https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3NCMNCMNBMNBMNBMNBMNB1A/d_I224x224x3NCMNCMNBMNBMNBMNBMNB1A.ell.zip">d_I224x224x3NCMNCMNBMNBMNBMNBMNB1A.ell.zip</a></td>
    </tr>
    <tr>
        <td> Accuracy </td>
        <td colspan="3"> ILSVRC2012: 62.63% (Top 5), 37.22% (Top 1) </td>
    </tr>
    <tr>
        <td> Performance </td>
        <td colspan="3"> Raspberry Pi 3 (Raspbian) @ 700MHz: 0.52s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 600MHz: 0.40s/frame<br>DragonBoard 410c @ 1.2GHz: 0.24s/frame </td>
    </tr>
    <tr>
        <td> Uncompressed Size </td>
        <td colspan="3"> 20MB </td>
    </tr>
    <tr>
        <td> Input </td>
        <td colspan="3"> 224 x 224 x {B,G,R} </td>
    </tr>
    <tr>
        <td rowspan="17"> Architecture </td>
        <tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 224x224x16</td>
	<td>size=3x3, stride=1, type=float32, activation=leaky relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 114x114x16</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 112x112x64</td>
	<td>size=3x3, stride=1, type=float32, activation=leaky relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 58x58x64</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 56x56x64</td>
	<td>size=3x3, stride=1, type=int64, activation=parametric relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 28x28x64</td>
	<td>size=3x3, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 28x28x128</td>
	<td>size=3x3, stride=1, type=int64, activation=parametric relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 14x14x128</td>
	<td>size=3x3, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 14x14x256</td>
	<td>size=3x3, stride=1, type=int64, activation=parametric relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 7x7x256</td>
	<td>size=3x3, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 7x7x512</td>
	<td>size=3x3, stride=1, type=int64, activation=parametric relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 4x4x512</td>
	<td>size=3x3, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 4x4x1024</td>
	<td>size=3x3, stride=1, type=int64, activation=parametric relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 4x4x1000</td>
	<td>size=1x1, stride=1, type=float32</td>
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

