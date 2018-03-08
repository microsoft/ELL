---
layout: default
title: ELL Gallery
permalink: /gallery/
datatable: true
---

<h1>The ELL Gallery</h1>
<p>Our gallery is a collection of bits and pieces that you can download and use in your projects.</p>
<h2>Pretrained Image Classification Models</h2>
<h3>ILSVRC2012</h3>
<p>We provide a collection of models trained on the dataset from the
ImageNet Large Scale Visual Recognition Challenge (ILSVRC2012). These
models use different neural net architectures on different size inputs
to trade off accuracy and speed. The plot below shows how each of the
models performs in terms of Top 1 accuracy (how often the most
confident prediction is right) versus speed (seconds per
frame). Click and drag to pan around. Zoom in and out with your
mouse's scrollwheel.</p>
<p>Here are the pretrained image classification models we provide. The
table can be sorted by column. Each model name is a link to a page
from which the model can be downloaded. The name contains both the
image size and an abbreviation for the model's architecture.</p>

<div id='plot'></div>
<script>
var windowWidth = Math.min(window.innerWidth
  , document.documentElement.clientWidth
  , document.body.clientWidth);
var width = Math.min(945, windowWidth);
var graphWidth = width - 100
var fontSize = 14
var spec = {
  "$schema": "https://vega.github.io/schema/vega-lite/v2.json",
  "title": "Top 1 accuracy vs milliseconds",
  "description": "A plot of accuracy versus performance",
  "width": graphWidth, 
  "height": graphWidth / 1.618, // golden ratio
  "config": {
    "title": {
      "fontSize": fontSize,
      "fontWeight": "normal"
    },
    "axis": {
      "labelFontSize": fontSize,
      "titleFontSize": fontSize
    },
    "legend" :{
      "titleFontSize": fontSize,
      "labelFontSize": fontSize,
      "titleFontWeight": "normal"
    }
  },
  "padding": {"left": 0, "top": 0, "right": 0, "bottom": 0},
  "autosize": {
    "type": "pad",
    "resize": true,
    "contains": "padding"
  },
  "data": {"values": {{site.data.all_models | jsonify}} },
  "transform": [
    {"calculate": "datum.secs_per_frame.pi3*1000", "as": "msecs_per_frame"}
  ],
  "selection": {
    "filter": {
        "type": "single",
        "fields": ["image_size"]
    },
    "grid": { "type": "interval", "bind": "scales" }
  },
  "mark": {"type":"point", "filled":true},
  "encoding": {
    "x": {"field": "msecs_per_frame", "type": "quantitative", "axis": {"title": ""} },
    "y": {"field": "accuracy.top1", "type": "quantitative", "axis": {"title": ""}, "scale": {"zero": false, "padding": 5} },
    "color": {
      "condition": {
        "selection": "filter",
        "field": "image_size",
        "type": "nominal",
        "legend": {"title": "Image Size", "orient": "bottom-right"}
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

<div class="table-responsive">
<table class="table table-striped table-bordered table-auto datatable" style="margin-left:auto;margin-right:auto;">
<thead>
<tr>
  <th>Image size</th>
  <th>Top 1<br>accuracy</th>
  <th>Top 5<br>accuracy</th>
  <th>msec/frame<br>on a Pi3</th>
  <th>Model name</th>
</tr>
</thead>
{% assign models = site.data.all_models %}
{% for model in models %}
  <tr>
    <td>{{model.image_size}}</td>
    <td style="text-align: right">{{model.accuracy.top1}}</td>
    <td style="text-align: right">{{model.accuracy.top5}}</td>
    <td style="text-align: right">{{model.secs_per_frame.pi3}}</td>
    <td><a href="/ELL/gallery/ILSVRC2012/{{model.name}}.html">{{model.friendly_name}}</a></td>
  </tr>
{% endfor %}
</table>
</div>
<h2>3D Designs</h2>
<ul>
  <li><a href="/ELL/gallery/Raspberry-Pi-3-Fan-Mount">Raspberry Pi 3 Fan Mount</a></li>
</ul>