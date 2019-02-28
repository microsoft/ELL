---
layout: default
title: ELL Gallery
permalink: /gallery/ILSVRC2012/
datatable: true
---

# Image classification models trained on ImageNet (ILSVRC2012)

This collection of image classification models is trained on the ImageNet Large Scale Visual Recognition Challenge dataset (ILSVRC2012). These models use different neural network architectures and different input sizes to trade off accuracy and speed. 

The plot below summarizes the accuracy and speed of each model. The model's accuracy is measured in terms of its *top 1 error rate* - how often is the model's top prediction correct. The model's speed is measured in milliseconds per input on a Raspberry Pi 3B, running at 700MHz, using only the quad-core ARM CPU (and not the VideoCore GPU). See [config.txt](https://github.com/Microsoft/ELL-PiDataCenter/blob/master/PiDataCenter/config.txt) for the boot config file used to configure the Raspberry Pi test machines.

<div id='plot'></div>

<script>
    var json_data = null;
    
    function resizeGraph() {
        showGraph(json_data);
    }

    function showGraph(data) {
        
        json_data = data
        plot = document.getElementById("plot");
        var graphWidth = plot.offsetWidth;
        var graphHeight = graphWidth / 1.618; // golden ratio
        var fontSize = 14
        var spec = {
        "$schema": "https://vega.github.io/schema/vega-lite/v3.json",
        "title": "Top 1 accuracy vs milliseconds",
        "description": "A plot of accuracy versus performance",
        "width": graphWidth, 
        "height": graphHeight,
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
            "type": "fit",
            "resize": true
        },
        "data": {
            "format": { "type": "json" },
            "values": data 
        },
        "selection": {
            "filter": {
                "type": "single",
                "fields": ["image_size"]
            },
            "grid": { "type": "interval", "bind": "scales" }
        },
        "mark": {"type":"point", "filled":true},
        "encoding": {
            "x": {"field": "msecs_per_frame", "type": "quantitative", "axis": {"title": "milliseconds / prediction"} },
            "y": {"field": "accuracy.top1", "type": "quantitative", "axis": {"title": "accuracy"}, "scale": {"zero": false, "padding": 5} },
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
            "tooltip": [{"field": "friendly_name", "type": "ordinal", "title": "name" },
                        {"field": "image_size", "type": "nominal", "title": "image size"},
                        {"field": "accuracy.top1", "type": "quantitative", "title": "accuracy"},
                        {"field": "msecs_per_frame", "type": "nominal", "title": "ms/frame"}],
            "size": {"value": 100}
        }
        }
        vegaEmbed("#plot", spec, {actions:false})
    }
    
    function fixupData(data) 
    {
        // show milliseconds instead of seconds for pi3 times.
        for (i = 0; i < data.length; i++)
        {
            row = data[i]
            speed = row['secs_per_frame']['pi3']
            row["msecs_per_frame"] = speed * 1000
        }
    }
    
    function updateTable(data) {
        var trHTML = '';
        $.each(data, function (i, item) {
            image_size = item['image_size'];
            accuracy1 = item['accuracy']["top1"];
            accuracy5 = item['accuracy']["top5"];
            speed = item['msecs_per_frame'];
            name = item['friendly_name'];
            url = "/ELL/gallery/ILSVRC2012/" + name + ".html";
            trHTML += '<tr><td>' + image_size + '</td><td>' + accuracy1 + '</td><td>' + accuracy5 + '</td><td>' + speed + '</td><td><a href=' + url + '>' + name + '</a></td></tr>';
        });        
        $('#tabledata').append(trHTML);
    }

    function loadGraph() {
        loadJSON('/ELL/gallery/ILSVRC2012/all_models.json', function(response) {
            // Parse JSON string into object
            data = JSON.parse(response);
            fixupData(data);
            showGraph(data);
            updateTable(data);
                    
            $('.datatable').DataTable({
                paging: false,
                aaSorting: [[0, 'asc'], [1, 'desc'], [3, 'asc']],
                autoWidth: true,
                searching: false,
                info: false
            });
        });
    }

    window.onload = function () {
        loadGraph();    
        window.addEventListener('resize', resizeGraph, false);
    }

</script>

The following table contains links to information pages that provide more detail about each model. Each information page includes a link to a `.ell` model file, which can be compiled and deployed using the ELL compiler. 

<div class="table-responsive">
<table id="tabledata" class="table table-striped table-bordered table-auto datatable" style="margin-left:auto;margin-right:auto;">
<thead>
<tr>
  <th>Image size</th>
  <th>Top 1<br>accuracy</th>
  <th>Top 5<br>accuracy</th>
  <th>msec/frame<br>on a Pi3</th>
  <th>Model name</th>
</tr>
</thead>
</table>
</div>
