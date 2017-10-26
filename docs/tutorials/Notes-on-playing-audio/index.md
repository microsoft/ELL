---
layout: default
title: Notes on playing audio
permalink: /tutorials/Notes-on-playing-audio/
---
# Notes on playing audio

*by Kern Handa*

## macOS

To play audio using ELL's `play_sound` function in the `tutorialHelpers` module
on macOS, the following command needs to be run:

```shell
pip install -U PyObjC
```

## Raspberry Pi

If an HDMI cable is used with the Raspberry Pi, an additional step is needed
to route audio through the headphone jack.

```shell
amixer cset numid=3 1
```

The default behavior is to go to `auto`, which tends to be HDMI. To reset to
the default behavior:

```shell
amixer cset numid=3 0
```
