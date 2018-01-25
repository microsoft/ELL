---
layout: default
title: ELL Gallery
permalink: /gallery/
datatable: true
---

# The ELL Gallery

Our gallery is a collection of bits and pieces that you can download and use in your projects.

<div id='plot'></div>
<script>
var spec = {
  "$schema": "https://vega.github.io/schema/vega-lite/v2.json",
  "description": "A plot of accuracy versus performance",
  "width": 500, "height": 500,
  "data": {"values": {{site.data.all_models | jsonify}} },
  "mark": {"type":"point", "filled":true},
  "encoding": {
    "x": {"field": "secs_per_frame.pi3", "type": "quantitative", "axis": {"title": "Seconds per frame"} },
    "y": {"field": "accuracy.top1", "type": "quantitative", "axis": {"title": "Top 1 accuracy"}},
    "color": {"field": "image_size", "type": "nominal", "legend": {"title": "Image Size"} },
    "shape": {"field": "image_size", "type": "nominal"},
    "tooltip": {"field": "directory", "type": "ordinal"},
    "size": {"value": 100}
  }
}
vegaEmbed("#plot", spec, {actions:false})
</script>

## Pretrained Image Classification Models

### ILSVRC2012

We provide a collection of models trained on the dataset from the
ImageNet Large Scale Visual Recognition Challenge (ILSVRC2012). These
models use different neural net architectures on different size inputs
to trade off accuracy and speed. The plot below shows how each of the
models performs in terms of Top 1 accuracy (how often the most
confident prediction is right) versus speed (seconds per
frame). Click and drag to pan around. Zoom in and out with your
mouse's scrollwheel.

<div id='plot'></div>
<script>
var windowWidth = window.innerWidth
  || document.documentElement.clientWidth
  || document.body.clientWidth;
var width = Math.min(600, windowWidth-100), height=width-100;
var spec = {
  "$schema": "https://vega.github.io/schema/vega-lite/v2.json",
  "description": "A plot of accuracy versus performance",
  "width": width, "height": height,
  "autosize": {
    "type": "fit",
    "resize": true
  },
  "data": {"values": {{site.data.all_models | jsonify}} },
  "selection": {
    "filter": {
        "type": "single",
        "fields": ["image_size"],
        "bind": {"input": "select", "name": "Input size to highlight ", "options": ["", "64\u00d764", "128\u00d7128", "256\u00d7256"]},
    },
    "grid": { "type": "interval", "bind": "scales" }
  },
  "mark": {"type":"point", "filled":true},
  "encoding": {
    "x": {"field": "secs_per_frame.pi3", "type": "quantitative", "axis": {"title": "Seconds per frame"} },
    "y": {"field": "accuracy.top1", "type": "quantitative", "axis": {"title": "Top 1 accuracy"}},
    "color": {
      "condition": {
        "selection": "filter",
        "field": "image_size",
        "type": "nominal",
        "legend": {"title": "Image Size"}
      },
      "value": "rgba(100,100,100,0.2)"
    },
    "shape": {"field": "image_size", "type": "nominal"},
    "tooltip": {"field": "directory", "type": "ordinal"},
    "size": {"value": 100}
  }
}
vegaEmbed("#plot", spec, {actions:false})
</script>
<br>

Here are the pretrained image classification models we provide. The
table can be sorted by column. Each model name is a link to a page
from which the model can be downloaded. The name contains both the
image size and an abbreviation for the model's architecture.

<table class="table table-striped table-bordered table-auto datatable">
<thead>
<tr>
  <th>Image size</th>
  <th>Top 1<br>accuracy</th>
  <th>Top 5<br>accuracy</th>
  <th>Sec/frame<br>on a Pi3</th>
  <th>Model name</th>
</tr>
</thead>
{% assign models = site.data.all_models | sort: 'image_size' %}
{% for model in models %}
  <tr>
    <td>{{model.image_size}}</td>
    <td style="text-align: right">{{model.accuracy.top1}}</td>
    <td style="text-align: right">{{model.accuracy.top5}}</td>
    <td style="text-align: right">{{model.secs_per_frame.pi3}}</td>
    <td><a href="/ELL/gallery/ILSVRC2012/{{model.name}}.html">{{model.name}}</a></td>
  </tr>
{% endfor %}
</table>

## 3D Designs

* [Raspberry Pi 3 Fan Mount](/ELL/gallery/Raspberry-Pi-3-Fan-Mount)
