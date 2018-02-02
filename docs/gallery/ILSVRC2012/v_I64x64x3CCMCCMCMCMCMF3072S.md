---
layout: default
title: 64x64x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/v_I64x64x3CCMCCMCMCMCMF3072S
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 64x64x3 Convolutional Neural Network (56.82% top 1 accuracy, 80.31% top 5 accuracy, 0.46s/frame on Raspberry Pi 3)

<table class="table table-striped table-bordered">
    <tr>
        <td> Download </td>
        <td colspan="3"> <a href="https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/v_I64x64x3CCMCCMCMCMCMF3072S/v_I64x64x3CCMCCMCMCMCMF3072S.ell.zip">v_I64x64x3CCMCCMCMCMCMF3072S.ell.zip</a></td>
    </tr>
    <tr>
        <td> Accuracy </td>
        <td colspan="3"> ILSVRC2012: 80.31% (Top 5), 56.82% (Top 1) </td>
    </tr>
    <tr>
        <td> Performance </td>
        <td colspan="3"> Raspberry Pi 3 (Raspbian) @ 700MHz: 0.46s/frame </td>
    </tr>
    <tr>
        <td> Uncompressed Size </td>
        <td colspan="3"> 177MB </td>
    </tr>
    <tr>
        <td> Input </td>
        <td colspan="3"> 64 &times; 64 &times; {B,G,R} </td>
    </tr>
    <tr>
        <td> Architecture </td>
        <td>
            <table class="arch-table">
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;64×64×64</td>
                    <td>size=3×3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;64×64×64</td>
                    <td>size=3×3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;32×32×64</td>
                    <td>size=2×2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;32×32×128</td>
                    <td>size=3×3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;32×32×128</td>
                    <td>size=3×3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;16×16×128</td>
                    <td>size=2×2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;16×16×256</td>
                    <td>size=3×3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;8×8×256</td>
                    <td>size=2×2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;8×8×512</td>
                    <td>size=3×3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;4×4×512</td>
                    <td>size=2×2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;4×4×512</td>
                    <td>size=3×3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;2×2×512</td>
                    <td>size=2×2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Softmax</td>
                    <td>&#8680;&nbsp;1×1×1000</td>
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
        <td colspan="3"> Trained by Juan Lema using CNTK 2.3.1 </td>
    </tr>
</table>

