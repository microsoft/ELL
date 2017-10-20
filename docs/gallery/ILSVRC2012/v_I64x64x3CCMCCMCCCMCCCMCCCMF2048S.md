---
layout: default
title: 64x64x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/v_I64x64x3CCMCCMCCCMCCCMCCCMF2048S
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 64x64x3 Convolutional Neural Network (57.12% top 1 accuracy, 80.49% top 5 accuracy, 0.79s/frame on Raspberry Pi 3)

<table class="table table-striped table-bordered">
    <tr>
        <td> Download </td>
        <td colspan="3"> <a href="https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/v_I64x64x3CCMCCMCCCMCCCMCCCMF2048S/v_I64x64x3CCMCCMCCCMCCCMCCCMF2048S.ell.zip">v_I64x64x3CCMCCMCCCMCCCMCCCMF2048S.ell.zip</a></td>
    </tr>
    <tr>
        <td> Accuracy </td>
        <td colspan="3"> ILSVRC2012: 80.49% (Top 5), 57.12% (Top 1) </td>
    </tr>
    <tr>
        <td> Performance </td>
        <td colspan="3"> Raspberry Pi 3 (Raspbian) @ 700MHz: 0.79s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 600MHz: 0.81s/frame<br>DragonBoard 410c @ 1.2GHz: 0.45s/frame </td>
    </tr>
    <tr>
        <td> Uncompressed Size </td>
        <td colspan="3"> 257MB </td>
    </tr>
    <tr>
        <td> Input </td>
        <td colspan="3"> 64 x 64 x {B,G,R} </td>
    </tr>
    <tr>
        <td> Architecture </td>
        <td>
            <table class="arch-table">
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;64x64x64</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;64x64x64</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;34x34x64</td>
                    <td>size=2x2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;32x32x128</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;32x32x128</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;18x18x128</td>
                    <td>size=2x2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;16x16x256</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;16x16x256</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;16x16x256</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;10x10x256</td>
                    <td>size=2x2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;8x8x512</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;8x8x512</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;8x8x512</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;6x6x512</td>
                    <td>size=2x2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;4x4x512</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;4x4x512</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;4x4x512</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;2x2x512</td>
                    <td>size=2x2,&nbsp;stride=2,&nbsp;operation=max</td>
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
        <td colspan="3"> Trained by Chuck Jacobs using CNTK 2.1 </td>
    </tr>
</table>

