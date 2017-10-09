---
layout: default
title: 128x128x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/d_I128x128x3CMCMCMCMCMCMC1A
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 128x128x3 Convolutional Neural Network (52.06% top 1 accuracy, 76.43% top 5 accuracy, 0.17s/frame on Raspberry Pi 3)

<table>
    <tr>
        <td> Download </td>
        <td colspan="3"> <a href="https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I128x128x3CMCMCMCMCMCMC1A/d_I128x128x3CMCMCMCMCMCMC1A.ell.zip">d_I128x128x3CMCMCMCMCMCMC1A.ell.zip</a></td>
    </tr>
    <tr>
        <td> Accuracy </td>
        <td colspan="3"> ILSVRC2012: 76.43% (Top 5), 52.06% (Top 1) </td>
    </tr>
    <tr>
        <td> Performance </td>
        <td colspan="3"> Raspberry Pi 3 (Raspbian) @ 700MHz: 0.17s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 600MHz: s/frame<br>DragonBoard 410c @ 1.2GHz: s/frame </td>
    </tr>
    <tr>
        <td> Uncompressed Size </td>
        <td colspan="3"> 38MB </td>
    </tr>
    <tr>
        <td> Input </td>
        <td colspan="3"> 128 x 128 x {B,G,R} </td>
    </tr>
    <tr>
        <td> Architecture </td>
        <td>
            <table class="arch-table">
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;128x128x16</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=leaky&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;64x64x16</td>
                    <td>size=2x2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;64x64x64</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=leaky&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;32x32x64</td>
                    <td>size=2x2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;32x32x64</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=leaky&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;16x16x64</td>
                    <td>size=2x2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;16x16x128</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=leaky&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;8x8x128</td>
                    <td>size=2x2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;8x8x256</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=leaky&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;4x4x256</td>
                    <td>size=2x2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;4x4x512</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=leaky&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;2x2x512</td>
                    <td>size=2x2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;2x2x1024</td>
                    <td>size=1x1,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=leaky&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;2x2x1000</td>
                    <td>size=1x1,&nbsp;stride=1,&nbsp;type=float32</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;1x1x1000</td>
                    <td>size=2x2,&nbsp;stride=1,&nbsp;operation=average</td>
                </tr>
                <tr class="arch-table">
                    <td>Softmax</td>
                    <td>&#8680;&nbsp;1x1x1000</td>
                    <td></td>
                </tr>
            </table>
        </td>
    </tr>
    <tr>
        <td> Output </td>
        <td colspan="3"> <a href="https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/categories.txt">ILSVRC2012 1000 classes</a> </td>
    </tr>
    <tr>
        <td> Notes </td>
        <td colspan="3"> Trained by Juan Lema using CNTK 2.1 </td>
    </tr>
</table>

