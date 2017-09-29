---
layout: default
title: 192x192x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/d_I192x192x3NCMNCMNBMNBMNBMNBMNC1A
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 192x192x3 Convolutional Neural Network (39.05% top 1 accuracy, 64.23% top 5 accuracy, 0.38s/frame on Raspberry Pi 3)

<table>
    <tr>
        <td> Download </td>
        <td colspan="3"> <a href="https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I192x192x3NCMNCMNBMNBMNBMNBMNC1A/d_I192x192x3NCMNCMNBMNBMNBMNBMNC1A.ell.zip">d_I192x192x3NCMNCMNBMNBMNBMNBMNC1A.ell.zip</a></td>
    </tr>
    <tr>
        <td> Accuracy </td>
        <td colspan="3"> ILSVRC2012: 64.23% (Top 5), 39.05% (Top 1) </td>
    </tr>
    <tr>
        <td> Performance </td>
        <td colspan="3"> Raspberry Pi 3 (Raspbian) @ 700MHz: 0.38s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 600MHz: 0.32s/frame<br>DragonBoard 410c @ 1.2GHz: 0.21s/frame </td>
    </tr>
    <tr>
        <td> Uncompressed Size </td>
        <td colspan="3"> 18MB </td>
    </tr>
    <tr>
        <td> Input </td>
        <td colspan="3"> 192 x 192 x {B,G,R} </td>
    </tr>
    <tr>
        <td rowspan="18"> Architecture </td>
        <tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 192x192x16</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 98x98x16</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 96x96x64</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 50x50x64</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 48x48x64</td>
	<td>size=3x3, stride=1, type=int64, activation=parametric relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 24x24x64</td>
	<td>size=3x3, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 24x24x128</td>
	<td>size=3x3, stride=1, type=int64, activation=parametric relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 12x12x128</td>
	<td>size=3x3, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 12x12x256</td>
	<td>size=3x3, stride=1, type=int64, activation=parametric relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 6x6x256</td>
	<td>size=3x3, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 6x6x512</td>
	<td>size=3x3, stride=1, type=int64, activation=parametric relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 3x3x512</td>
	<td>size=3x3, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 3x3x1024</td>
	<td>size=3x3, stride=1, type=int64, activation=parametric relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 2x2x1024</td>
	<td>size=3x3, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 2x2x1000</td>
	<td>size=1x1, stride=1, type=float32</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 1x1x1000</td>
	<td>size=2x2, stride=1, operation=average</td>
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

