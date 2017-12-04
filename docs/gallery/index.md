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

<table class="table table-striped table-bordered datatable">
<thead>
<tr>
  <th>Image size</th>
  <th>Top 1 accuracy</th>
  <th>Top 5 accuracy</th>
  <th>Sec/frame on a Pi3</th>
  <th>Model name</th>
</tr>
</thead>
{% assign models = site.data.all_models | sort: 'image_size' %}
{% for model in models %}
  <tr>
    <td>{{model.image_size}} x {{model.image_size}} x 3</td>
    <td style="text-align: right">{{model.accuracy.top1}}</td>
    <td style="text-align: right">{{model.accuracy.top5}}</td>
    <td style="text-align: right">{{model.secs_per_frame.pi3}}</td>
    <td><a href="/ELL/gallery/ILSVRC2012/{{model.directory}}.md">{{model.directory}}</a></td>
  </tr>
{% endfor %}
</table>

## 3D Designs

* [Raspberry Pi 3 Fan Mount](/ELL/gallery/Raspberry-Pi-3-Fan-Mount)
