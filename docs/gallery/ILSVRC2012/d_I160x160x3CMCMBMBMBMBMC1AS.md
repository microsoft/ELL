---
layout: default
title: 160x160x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/d_I160x160x3CMCMBMBMBMBMC1AS
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 160x160x3 Convolutional Neural Network (35.75% top 1 accuracy, 60.76% top 5 accuracy, 0.29s/frame on Raspberry Pi 3)

<table class="table table-striped table-bordered">
    <tr>
        <td> Download </td>
        <td colspan="3"> <a href="https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I160x160x3CMCMBMBMBMBMC1AS/d_I160x160x3CMCMBMBMBMBMC1AS.ell.zip">d_I160x160x3CMCMBMBMBMBMC1AS.ell.zip</a></td>
    </tr>
    <tr>
        <td> Accuracy </td>
        <td colspan="3"> ILSVRC2012: 60.76% (Top 5), 35.75% (Top 1) </td>
    </tr>
    <tr>
        <td> Performance </td>
        <td colspan="3"> Raspberry Pi 3 (Raspbian) @ 700MHz: 0.29s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 600MHz: 0.22s/frame<br>DragonBoard 410c @ 1.2GHz: 0.13s/frame </td>
    </tr>
    <tr>
        <td> Uncompressed Size </td>
        <td colspan="3"> 17MB </td>
    </tr>
    <tr>
        <td> Input </td>
        <td colspan="3"> 160 x 160 x {B,G,R} </td>
    </tr>
    <tr>
        <td> Architecture </td>
        <td>
            <table class="arch-table">
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;160x160x16</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;82x82x16</td>
                    <td>size=2x2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;80x80x64</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;42x42x64</td>
                    <td>size=2x2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;40x40x64</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=int64,&nbsp;activation=parametric&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;20x20x64</td>
                    <td>size=3x3,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;20x20x128</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=int64,&nbsp;activation=parametric&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;10x10x128</td>
                    <td>size=3x3,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;10x10x256</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=int64,&nbsp;activation=parametric&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;5x5x256</td>
                    <td>size=3x3,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;5x5x512</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=int64,&nbsp;activation=parametric&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;3x3x512</td>
                    <td>size=3x3,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;3x3x1024</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=int64,&nbsp;activation=parametric&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;2x2x1024</td>
                    <td>size=3x3,&nbsp;stride=2,&nbsp;operation=max</td>
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
        <td colspan="3">Trained by Juan Lema
 using CNTK 2.2 </td>
    </tr>
</table>

