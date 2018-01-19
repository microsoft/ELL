---
layout: default
title: 128x128x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/dscs2_I128x128x3CCCCCCC1AS
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 128x128x3 Convolutional Neural Network (45.32% top 1 accuracy, 69.63% top 5 accuracy, 0.19s/frame on Raspberry Pi 3)

<table class="table table-striped table-bordered">
    <tr>
        <td> Download </td>
        <td colspan="3"> <a href="https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/dscs2_I128x128x3CCCCCCC1AS/dscs2_I128x128x3CCCCCCC1AS.ell.zip">dscs2_I128x128x3CCCCCCC1AS.ell.zip</a></td>
    </tr>
    <tr>
        <td> Accuracy </td>
        <td colspan="3"> ILSVRC2012: 69.63% (Top 5), 45.32% (Top 1) </td>
    </tr>
    <tr>
        <td> Performance </td>
        <td colspan="3"> Raspberry Pi 3 (Raspbian) @ 700MHz: 0.19s/frame </td>
    </tr>
    <tr>
        <td> Uncompressed Size </td>
        <td colspan="3"> 59MB </td>
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
                    <td>&#8680;&nbsp;64x64x32</td>
                    <td>size=5x5,&nbsp;stride=2,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;32x32x64</td>
                    <td>size=5x5,&nbsp;stride=2,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;16x16x128</td>
                    <td>size=5x5,&nbsp;stride=2,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;8x8x256</td>
                    <td>size=5x5,&nbsp;stride=2,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;4x4x512</td>
                    <td>size=5x5,&nbsp;stride=2,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;4x4x1000</td>
                    <td>size=1x1,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;1x1x1000</td>
                    <td>size=4x4,&nbsp;stride=1,&nbsp;operation=average</td>
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

