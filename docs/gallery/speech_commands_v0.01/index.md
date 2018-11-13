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
            "href": {"field": "url", "type": "nominal"}
        }
        }
        vegaEmbed("#plot", spec, {actions:false})
    }
    function loadJSON(filename, callback) {
        var xobj = new XMLHttpRequest();
            xobj.overrideMimeType("application/json");
        xobj.open('GET', filename, true);
        xobj.onreadystatechange = function () {
            if (xobj.readyState == 4 && xobj.status == "200") {
                // Required use of an anonymous callback as .open will NOT return a value but simply returns undefined in asynchronous mode
                callback(xobj.responseText);
            }
        };
        xobj.send(null);  
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
    function loadGraph() {
        loadJSON('/ELL/gallery/speech_commands_v0.01/pareto.json', function(response) {
            // Parse JSON string into object
            data = JSON.parse(response);
            data = filterData(data);
            showGraph(data)
        });
    }

window.onload = function () {
    loadGraph();    
    window.addEventListener('resize', resizeGraph, false);
}

</script>
