---
layout: default
title: 192x192x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/v_I192x192x3CCMCCMCCCMCCCMCCCMF1024F1024
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 192x192x3 Convolutional Neural Network (63.50% top 1 accuracy, 85.33% top 5 accuracy, 6.88s/frame on Raspberry Pi 3)

<table>
    <tr>
        <td> Download </td>
        <td colspan="3"> <a href="https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/v_I192x192x3CCMCCMCCCMCCCMCCCMF1024F1024/v_I192x192x3CCMCCMCCCMCCCMCCCMF1024F1024.ell.zip">v_I192x192x3CCMCCMCCCMCCCMCCCMF1024F1024.ell.zip</a></td>
    </tr>
    <tr>
        <td> Accuracy </td>
        <td colspan="3"> ILSVRC2012: 85.33% (Top 5), 63.50% (Top 1) </td>
    </tr>
    <tr>
        <td> Performance </td>
        <td colspan="3"> Raspberry Pi 3 (Raspbian) @ 700MHz: 6.88s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 600MHz: 6.54s/frame<br>DragonBoard 410c @ 1.2GHz: 6.09s/frame </td>
    </tr>
    <tr>
        <td> Uncompressed Size </td>
        <td colspan="3"> 443MB </td>
    </tr>
    <tr>
        <td> Input </td>
        <td colspan="3"> 192 x 192 x {B,G,R} </td>
    </tr>
    <tr>
        <td rowspan="20"> Architecture </td>
        <tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 192x192x64</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 192x192x64</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 98x98x64</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 96x96x128</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 96x96x128</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 50x50x128</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 48x48x256</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 48x48x256</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 48x48x256</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 26x26x256</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 24x24x512</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 24x24x512</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 24x24x512</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 14x14x512</td>
	<td>size=2x2, stride=2, operation=max</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 12x12x512</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 12x12x512</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Convolution</td>
	<td>&#8680; 12x12x512</td>
	<td>size=3x3, stride=1, type=float32, activation=relu</td>
</tr>
<tr class="table-row-condensed">
	<td>Pooling</td>
	<td>&#8680; 6x6x512</td>
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

