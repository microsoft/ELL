---
layout: home
title: The Embedded Learning Library
---

The Embedded Learning Library (ELL) allows you to design and deploy intelligent machine-learned models onto resource constrained platforms and small single-board computers, like Raspberry Pi, Arduino, and micro:bit. The deployed models run locally, without requiring a network connection and without relying on servers in the cloud. ELL is an early preview of the embedded AI and machine learning technologies developed at Microsoft Research.

We built ELL for makers, technology enthusiasts, students, entrepreneurs, and developers who aspire to build intelligent devices and AI-powered gadgets. Our tools, our code, and all of the other resources available on this website are free for anyone to adapt and use (for details, see licensing below). Just keep in mind that ELL is a work in progress and that we expect it change rapidly, including breaking API changes.

ELL is a software library and an accompanying set of software tools, written in modern C++, with an optional interface in Python. Download ELL from our [GitHub repository](https://github.com/Microsoft/ELL), either as a [zip file](https://github.com/Microsoft/ELL/archive/master.zip), or with the following command:

```shell
git clone https://github.com/Microsoft/ELL.git
```

While the goal of ELL is to deploy software onto resource constrained platforms and small single-board computers, most of the interaction with ELL occurs on a laptop or desktop computer (Windows, Ubuntu Linux, or macOS). Technically, you can think of ELL as a cross-compiler for embedded intelligence - the compiler itself runs on your laptop or desktop computer and the machine code that it generates runs on your single-board computer.

## Installation and Setup
Install ELL on a
[Windows](https://github.com/Microsoft/ELL/blob/master/INSTALL-Windows.md), [Ubuntu Linux](https://github.com/Microsoft/ELL/blob/master/INSTALL-Ubuntu.md), or [macOS](https://github.com/Microsoft/ELL/blob/master/INSTALL-Mac.md)
laptop or desktop computer. If you intend to deploy models onto a Raspberry Pi, follow our instruction on [setting up the Raspberry Pi](/ELL/tutorials/Setting-up-your-Raspberry-Pi).

## Getting Started
A great place to start is our [tutorials section](/ELL/tutorials). As we develop and release new functionality in ELL, we also publish tutorials that showcase that functionality. Currently, our tutorials are focused on simple embedded computer vision tasks on Raspberry Pi, but we expect the scope to grow with time. Have fun!

## The ELL Gallery
Our [gallery](/ELL/gallery) is a collection of bits and pieces that you can download and use in your projects. Currently, the gallery includes a handful of pre-trained computer vision models and instructions for 3D printing an [active cooling attachment](/ELL/gallery/Raspberry-Pi-3-Fan-Mount) for your Raspberry Pi.

## License
The ELL code and sample code in our tutorials are released under the [MIT Open Source License](https://github.com/Microsoft/ELL/blob/master/LICENSE.txt). Some of the other content on this website, such as the 3D models in our gallery, are released under the [Creative Commons Attribution 4.0 license](https://creativecommons.org/licenses/by/4.0/).

## Conduct and Privacy
ELL has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information on this code of conduct, see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments. Read Microsoft's statement on [Privacy & Cookies](https://privacy.microsoft.com/en-us/privacystatement/).
