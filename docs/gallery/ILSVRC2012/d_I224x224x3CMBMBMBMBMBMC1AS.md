---
layout: default
title: 224x224x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/d_I224x224x3CMBMBMBMBMBMC1AS
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 224x224x3 Convolutional Neural Network (36.52% top 1 accuracy, 60.89% top 5 accuracy, 0.63s/frame on Raspberry Pi 3)

<table class="table table-striped table-bordered">
    <tr>
        <td> Download </td>
        <td colspan="3"> <a href="https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3CMBMBMBMBMBMC1AS/d_I224x224x3CMBMBMBMBMBMC1AS.ell.zip">d_I224x224x3CMBMBMBMBMBMC1AS.ell.zip</a></td>
    </tr>
    <tr>
        <td> Accuracy </td>
        <td colspan="3"> ILSVRC2012: 60.89% (Top 5), 36.52% (Top 1) </td>
    </tr>
    <tr>
        <td> Performance </td>
        <td colspan="3"> Raspberry Pi 3 (Raspbian) @ 700MHz: 0.63s/frame<br>Raspberry Pi 3 (OpenSUSE) @ 600MHz: 0.42s/frame<br>DragonBoard 410c @ 1.2GHz: 0.23s/frame </td>
    </tr>
    <tr>
        <td> Uncompressed Size </td>
        <td colspan="3"> 30MB </td>
    </tr>
    <tr>
        <td> Input </td>
        <td colspan="3"> 224 x 224 x {B,G,R} </td>
    </tr>
    <tr>
        <td> Architecture </td>
        <td>
            <table class="arch-table">
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;224x224x16</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;114x114x16</td>
                    <td>size=2x2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;112x112x64</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=int64,&nbsp;activation=parametric&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;56x56x64</td>
                    <td>size=3x3,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;56x56x64</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=int64,&nbsp;activation=parametric&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;28x28x64</td>
                    <td>size=3x3,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;28x28x128</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=int64,&nbsp;activation=parametric&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;14x14x128</td>
                    <td>size=3x3,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;14x14x256</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=int64,&nbsp;activation=parametric&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;7x7x256</td>
                    <td>size=3x3,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;7x7x512</td>
                    <td>size=3x3,&nbsp;stride=1,&nbsp;type=int64,&nbsp;activation=parametric&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;4x4x512</td>
                    <td>size=3x3,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;4x4x1024</td>
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

