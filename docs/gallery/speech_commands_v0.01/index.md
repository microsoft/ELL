---
layout: default
title: ELL Audio Gallery
permalink: /gallery/speech_commands_v0.01/
datatable: true
--- 

<h2>Speech Commands Model Gallery</h2>

<p>This gallery is a collection of audio keyword spotter models trained using the technique described in the <a href='/ELL/tutorials/Training-audio-keyword-spotter-with-pytorch/'>Tutorial on training an audio keyword spotter</a> using the 
<a href='http://download.tensorflow.org/data/speech_commands_v0.01.tar.gz'>Speech Commands Dataset</a>.

<p>
These models use different neural network architectures with different sizes to trade off accuracy and speed. The plot below shows how each of the
models performs in terms of accuracy (how often the most confident prediction is right) versus speed (milliseconds per audio prediction).
Click and drag to pan around. Zoom in and out with your mouse's scrollwheel.  Click on a model to go to the gallery page where you can download
that model.</p>
<br/>
<div id='plot' ></div>

<script language="javascript">

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
        "title": "accuracy vs milliseconds",
        "description": "A plot of accuracy versus performance",
        "width": graphWidth, 
        "height": graphHeight,
        "data": {
            "format": { "type": "json" },
            "values": data 
        },    
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
            "resize": true,
        },
        "selection": {
            "grid": { "type": "interval", "bind": "scales" }
        },
        "mark": {"type":"point", "filled":true, "tooltip": {"content": "data"}},
        "transform": [{
            "calculate": "'https://github.com/Microsoft/ELL-models/tree/master/models/speech_commands_v0.01/' + datum.friendly_name", "as": "url"
        }],
        "encoding": {
            "x": {"field": "predict_time", "type": "quantitative", "axis": {"title": "milliseconds / prediction"}, "scale": {"padding": 0, "zero": false} },
            "y": {"field": "accuracy_test", "type": "quantitative", "axis": {"title": "accuracy"}, "scale": {"padding": 0, "zero": false}}, 
            "color": {"field": "architecture", "type": "nominal", "legend": {"title": "Architecture", "orient": "bottom-right" } },
            "shape": { "field": "hidden_size", "legend" : null },
            "size": {"value": 100},
            "href": {"field": "url", "type": "nominal"},            
            "tooltip": [{"field": "friendly_name", "type": "ordinal", "title": "name" },
                        {"field": "architecture", "type": "nominal", "title": "architecture"},
                        {"field": "accuracy_test", "type": "quantitative", "title": "accuracy"},
                        {"field": "predict_time", "type": "nominal", "title": "ms/frame"},
                        {"field": "input_size", "type": "quantitative", "title": "input size" },
                        {"field": "hidden_size", "type": "quantitative", "title": "hidden size" },
                        {"field": "num_layers", "type": "quantitative", "title": "num layers" }],
        }
        }
        vegaEmbed("#plot", spec, {actions:false})
    }
    
    function filterData(data) {
        var filtered = []
        for (i = 0; i < data.length; i++)
        {
            row = data[i]
            rate = row['sample_rate']
            if (rate == 8000) {
                rate = "8kHz"
            } else if (rate == 16000) {
                rate = "16kHz"
            }
            row['architecture'] = row['architecture'] + " " + rate
            include = true // !row['reset'] && row['vad'] == true && !row['hard_sigmoid']
            // include = !row['reset'] && row['vad'] == true && !row['hard_sigmoid']
            if (include) {
                filtered.push(row)
            }
        }
        return filtered;
    }
    
    function updateTable(data) {
        var trHTML = '';
        $.each(data, function (i, item) {
            architecture = item['architecture'];
            hidden_size = item['hidden_size'];
            num_layers = item['num_layers'];
            accuracy = (100 * item['accuracy_test']).toFixed(2);
            speed = item['predict_time'];
            name = item['friendly_name'];
            url = "https://github.com/Microsoft/ELL-models/tree/master/models/speech_commands_v0.01/" + name;
            trHTML += '<tr><td>' + architecture + '</td><td>' + accuracy + '</td><td>' + speed + '</td><td><a href=' + url + '>' + name + '</a></td></tr>';
        });        
        $('#tabledata').append(trHTML);
    }

    function loadGraph() {
        loadJSON('/ELL/gallery/speech_commands_v0.01/pareto.json', function(response) {
            // Parse JSON string into object
            data = JSON.parse(response);
            data = filterData(data);
            showGraph(data);
            updateTable(data);
                
            $('.datatable').DataTable({
                paging: false,
                aaSorting: [[2, 'asc'], [1, 'desc']], // ascending on accuracy, descending on time.
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

<br/>
<br/>
<div class="table-responsive">
<table id="tabledata" class="table table-striped table-bordered table-auto datatable" style="margin-left:auto;margin-right:auto;">
<thead>
<tr>
  <th>Architecture</th>
  <th>Accuracy</th>
  <th>msec/frame<br>on a PC</th>
  <th>Model name</th>
</tr>
</thead>
</table>
</div>
