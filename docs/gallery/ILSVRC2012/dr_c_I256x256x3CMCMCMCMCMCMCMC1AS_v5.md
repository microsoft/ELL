---
layout: default
title: 256x256x3 Convolutional Neural Network
permalink: /gallery/ILSVRC2012/dr_c_I256x256x3CMCMCMCMCMCMCMC1AS_v5
---

[Back to Gallery](/ELL/gallery)

## ILSVRC2012 Classification: 256x256x3 Convolutional Neural Network (63.83% top 1 accuracy, 85.21% top 5 accuracy, 1.13s/frame on Raspberry Pi 3)

<table class="table table-striped table-bordered">
    <tr>
        <td> Download </td>
        <td colspan="3"> <a href="https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/dr_c_I256x256x3CMCMCMCMCMCMCMC1AS_v5/dr_c_I256x256x3CMCMCMCMCMCMCMC1AS_v5.ell.zip">dr_c_I256x256x3CMCMCMCMCMCMCMC1AS_v5.ell.zip</a></td>
    </tr>
    <tr>
        <td> Accuracy </td>
        <td colspan="3"> ILSVRC2012: 85.21% (Top 5), 63.83% (Top 1) </td>
    </tr>
    <tr>
        <td> Performance </td>
        <td colspan="3"> Raspberry Pi 3 (Raspbian) @ 700MHz: 1.13s/frame </td>
    </tr>
    <tr>
        <td> Uncompressed Size </td>
        <td colspan="3"> 212MB </td>
    </tr>
    <tr>
        <td> Input </td>
        <td colspan="3"> 256 &times; 256 &times; {B,G,R} </td>
    </tr>
    <tr>
        <td> Architecture </td>
        <td>
            <table class="arch-table">
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;256×256×24</td>
                    <td>size=3×3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=leaky&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;130×130×24</td>
                    <td>size=2×2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;128×128×48</td>
                    <td>size=3×3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=leaky&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;66×66×48</td>
                    <td>size=2×2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;64×64×96</td>
                    <td>size=3×3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=leaky&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;34×34×96</td>
                    <td>size=2×2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;32×32×192</td>
                    <td>size=3×3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=leaky&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;18×18×192</td>
                    <td>size=2×2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;16×16×384</td>
                    <td>size=3×3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=leaky&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;10×10×384</td>
                    <td>size=2×2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;8×8×768</td>
                    <td>size=3×3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=leaky&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;6×6×768</td>
                    <td>size=2×2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;4×4×1536</td>
                    <td>size=3×3,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=leaky&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;2×2×1536</td>
                    <td>size=2×2,&nbsp;stride=2,&nbsp;operation=max</td>
                </tr>
                <tr class="arch-table">
                    <td>Convolution</td>
                    <td>&#8680;&nbsp;2×2×1000</td>
                    <td>size=1×1,&nbsp;stride=1,&nbsp;type=float32,&nbsp;activation=leaky&nbsp;relu</td>
                </tr>
                <tr class="arch-table">
                    <td>Pooling</td>
                    <td>&#8680;&nbsp;1×1×1000</td>
                    <td>size=2×2,&nbsp;stride=1,&nbsp;operation=average</td>
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

