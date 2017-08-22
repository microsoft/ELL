// this function turns column data into typed javascript objects 
function objectify_columns(d, _, columns) {
    d.x = parseFloat(d.x);
    for (var i = 1, n = columns.length, c; i < n; ++i) d[c = columns[i]] = +d[c];
    return d;
}

// draw SVG plot of multi-line series data stored in the given tab delimited file.
// The first column is the x-axis, the subsequent columns will be drawn as lines
// in a multi-line plot.  It supports up to 10 lines using d3.schemeCategory10 colors.
// The optional xRange and yRange arguments provide overrides for automatic calculation
// of the desired range for x and y axes.  Pass null if you don't want to override them.
// The optional colors argument provides an array of colors to use for each series.
// This argument defaults to d3.schemeCategory10.  The optional xTickInterval allows
// you to define the tick interval (default is usually a power of 10).
function multi_line_plot(tsvFileName, svgElementId, yAxisLabel, xRange, yRange, xTickInterval, colors) {
    // find the <svg> element, and set the margins
    
    var se = document.getElementById(svgElementId);
    var svgRect = se.getBoundingClientRect();

    var svg = d3.select(se),
        margin = { top: 20, right: 80, bottom: 30, left: 30 },
        width = svgRect.width - margin.left - margin.right,
        height = svgRect.height - margin.top - margin.bottom;

    // add svg group
    var g = svg.append("g").attr("transform", "translate(" + margin.left + "," + margin.top + ")");

    if (!Array.isArray(colors)) {
        colors = d3.schemeCategory10
    }

    // define the x, y axes and a z axis with 10 colors for the multi line series 
    var xScale = d3.scaleLinear().range([0, width]),
        yScale = d3.scaleLinear().range([height, 0]),
        seriesColor = d3.scaleOrdinal(colors);

    // lambda for generating lines from given data containing 'x' and 'y' values
    var line = d3.line()
        .curve(d3.curveBasis)
        .x(function(d) { return xScale(d.x); })
        .y(function(d) { return yScale(d.y); });

    // Load the tab separated data, objectify it, then when loaded, generate the svg plot.
    d3.tsv(tsvFileName, objectify_columns, function(error, data) {

        if (error) throw error;

        // create series by id containing x, y values
        var series = data.columns.slice(1).map(function(id) {
            return {
                id: id,
                values: data.map(function(d) {
                    return { x: d.x, y: d[id] };
                })
            };
        });

        // x axis uses .x property of data values.
        if (Array.isArray(xRange)) {
            xScale.domain(xRange);
            xScale.clamp(true);
        } else {
            xScale.domain(d3.extent(data, function(d) { return d.x; }));
        }

        if (typeof(xTickInterval) == 'number') {
            // ok, user knows best, so use the given tick interval...
            xScale.ticks = function(count) {
                d = xScale.domain();
                ticks = [];
                len = d[1];
                for (i = 0; i < len; i += xTickInterval) {
                    ticks.push(i);
                }
                ticks.push(len);
                return ticks;
            }
        }

        if (Array.isArray(yRange)) {
            yScale.domain(yRange);
            yScale.clamp(true);
        } else {
            // Compute the min/max over all y values in all series
            yScale.domain([
                d3.min(series, function(c) { return d3.min(c.values, function(d) { return d.y; }); }),
                d3.max(series, function(c) { return d3.max(c.values, function(d) { return d.y; }); })
            ]);
        }

        // the colors are mapped to the id of each series
        seriesColor.domain(series.map(function(c) { return c.id; }));

        // add the x-axis line
        g.append("g")
            .attr("class", "axis axis--x")
            .attr("transform", "translate(0," + height + ")")
            .call(d3.axisBottom(xScale));

        // add the y-axis line
        g.append("g")
            .attr("class", "axis axis--y")
            .call(d3.axisLeft(yScale))
            .append("text")
            .attr("transform", "rotate(-90)")
            .attr("y", 6)
            .attr("dy", "0.71em")
            .attr("fill", "#000")
            .style("font", "16px sans-serif")
            .text(yAxisLabel);

        // add an svg group for each line in the series
        var seriesGroup = g.selectAll(".seriesGroup")
            .data(series)
            .enter().append("g")
            .attr("class", "seriesGroup");

        // add the svg path for each series using previous line lambda and color from seriesColor.
        seriesGroup.append("path")
            .attr("class", "line")
            .attr("d", function(d) { return line(d.values); })
            .style("stroke", function(d) { return seriesColor(d.id); });

        // add labels for each series line
        seriesGroup.append("text")
            .datum(function(d) { return { id: d.id, data: d.values[d.values.length - 1] }; })
            .attr("transform", function(d) { return "translate(" + xScale(d.data.x) + "," + yScale(d.data.y) + ")"; })
            .attr("x", 3)
            .attr("dy", "0.35em")
            .style("font", "16px sans-serif")
            .text(function(d) { return d.id; });
    });

}