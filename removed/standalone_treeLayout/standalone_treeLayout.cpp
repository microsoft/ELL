////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     standalone_treeLayout.cpp (standalone_treeLayout)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "QpLayoutGenerator.h"

#include <algorithm>
#include <stdexcept>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>

using namespace treeLayout; // TODO remove this
using namespace std;

struct node
{
    int child0;
    int child1;
    double output = 1.0;

    node(int child0, int child1, double output) : child0(child0), child1(child1), output(output) {}

    int GetChild0() const
    {
        return child0;
    }

    int GetChild1() const
    {
        return child1;
    }

    double get_output() const
    {
        return output;
    }
};

struct tree_info
{
    tree_info(const vector<node>& nodes, const vector<node>& interior_nodes, const Layout& node_positions) : nodes(nodes), interior_nodes(interior_nodes), node_positions(node_positions) {};

    vector<node> nodes;
    vector<node> interior_nodes;
    Layout node_positions;
};

vector<node> read_nodes(istream& is, bool bdt_format)
{
    if (bdt_format)
    {
        string dummy;
        getline(is, dummy);
    }

    string child0_str, child1_str;
    string output_str;

    getline(is, child0_str);
    getline(is, child1_str);
    if (bdt_format)
    {
        getline(is, output_str);
    }

    stringstream child0_sstream(child0_str);
    stringstream child1_sstream(child1_str);
    stringstream output_sstream(output_str);

    int child0, child1;
    double output = 1.0;

    vector<node> result;
    while (child0_sstream && child1_sstream)
    {
        child0_sstream >> child0;
        child1_sstream >> child1;

        if (!child0_sstream || !child1_sstream)
        {
            break;
        }

        if (bdt_format)
        {
            output_sstream >> output;
        }

        node c{ child0, child1, output };
        result.push_back(c);
    } 

    return result;
}

void write_header(const QpLayoutGenerator::Params& p, ostream& os, bool use_html)
{
    if (use_html)
    {
        os << "<!DOCTYPE html>" << endl << "<html>" << endl;
        
        // write parameter values used to generate ensemble
        os << "<!--" << endl;
        os << "gdNumSteps: " << p.gdNumSteps << endl;
        os << "gdLearningRate: " << p.gd_learning_rate << endl;
        os << "depthSpace: " << p.depthSpace << endl;
        os << "depthSpaceGrowthFactor: " << p.depthSpaceGrowthFactor << endl;
        os << "offsetSpace: " << p.offsetSpace << endl;
        os << "offsetSpaceGrowthFactor: " << p.offsetSpaceGrowthFactor << endl;
        os << "springCoeffGrowthFactor: " << p.spring_coeff_growth << endl;
        os << "springRestLength: " << p.springRestLength << endl;
        os << "-->" << endl;

        os << "<head>" << endl;
        os << R"(
<style type="text/css">
    body { 
        background:#fff; 
        margin:8 
        }
    #zoomPanel { 
    visibility: visible;
    background: white;
    position: absolute;
    width: 100%;
    height: 100%;
    }

    #closeButton { position:absolute;}

    div.container {
    }
    svg {
        display:inline-block; 
        /*border:1px solid #ccc; */
        /*position:absolute;*/
        }

    svg line { 
        stroke: black;
        stroke-width: 3.0; }

    svg circle { fill: blue; }
    svg circle { 
        stroke: black; 
        stroke-width: 3.0; }
    svg circle.pruned { fill: #fff; }

    ol { counter-Reset: item; }
    ol li { 
        display: inline-block; 
        border-bottom: 1px solid #eee;
        margin-bottom: 16px;
        padding: 10px;
        }
    ol li:before { 
        content:counter(item); 
        counter-increment: item; 
        font: 12pt Verdana, sans-serif;
        }
</style>
)";

        os << R"(

    <script type="text/javascript" src="https://ajax.aspnetcdn.com/ajax/jquery/jquery-1.8.3.serializer"></script>

<script type="text/javascript">

    gZoomLevel = 0;
    gMinZoomLevel = -4;
    gMaxZoomLevel = 4;

  $(document).ready(function () {
  $("#zoomPanel").hide();
  $("#zoomIn").click(zoomIn);
  $("#zoomOut").click(zoomOut);
  $("svg").click(zoomItem);
  $("#closeButton").click(unzoomItem);
  $(window).resize(resizeZoom);


    // Scan through all SVG elements and cache inital size in object (via jQuery data() call or something). Alternately, set data-width/data-height attributes when generating objects in the first place.
    $("svg").each(function(){
        var $this = $(this);
    // problem: $this.attr('width') is returning undefined...
    var initialWidth = $this.attr('width');
    var initialHeight = $this.attr('height');
        $this.data('initial-width', $this.attr('width'));
        $this.data('initial-height', $this.attr('height'));
});
    });

  
    function zoomIn(){
    var $this = $(this);
        gZoomLevel = Math.min(gZoomLevel+1, gMaxZoomLevel);
        resizeZoomedElements();

    };
    
    function zoomOut(){
    var $this = $(this);
        gZoomLevel = Math.max(gZoomLevel-1, gMinZoomLevel);
        resizeZoomedElements();
    };
    
    function resizeZoomedElements() {
        $("svg").each(function(){
        var $this = $(this);
        var initialWidth = $this.data('initial-width');
        var initialHeight = $this.data('initial-height');
        var zoom = Math.pow(2, gZoomLevel);
        var newWidth = zoom * initialWidth;
        var newHeight = zoom * initialHeight;
        $this.attr('width', newWidth);
        $this.attr('height', newHeight);
    });
    }

    function zoomItem() {
  var zoomPanel = $("#zoomPanel")

  var zoomTreeContainer = $("#zoomTreeContainer")
  var tree = $(this).clone();

  var panelWidth = zoomPanel.width();
  var panelHeight = zoomPanel.height();
  var origWidth = tree.attr('width');
  var origHeight = tree.attr('height');
  var xScale = panelWidth / origWidth;
  var yScale = panelHeight / origHeight;
  var Scale = Math.min(xScale, yScale);
  var newWidth = Scale*origWidth;
  var newHeight = Scale*origHeight;
  tree.attr('width', newWidth).attr('height', newHeight);

    zoomTreeContainer.empty();
    zoomTreeContainer.append(tree);

    zoomPanel.show();
  zoomPanel.resize(resizeZoom);
  $(".container").hide();
    }

    function unzoomItem() {
    var $this = $(this);
  $("#zoomTreeContainer").empty();
    $("#zoomPanel").hide();
  $(".container").show();
    }

  function resizeZoom() {

  var zoomPanel = $("#zoomPanel")
  var tree = $("#zoomPanel svg");
if(zoomPanel.is(':visible') && tree) {
  var panelWidth = zoomPanel.width();
  var panelHeight = zoomPanel.height();
  var origWidth = tree.attr('width');
  var origHeight = tree.attr('height');
  var xScale = panelWidth / origWidth;
  var yScale = panelHeight / origHeight;
  var Scale = Math.min(xScale, yScale);
  var newWidth = Scale*origWidth;
  var newHeight = Scale*origHeight;
  tree.attr('width', newWidth).attr('height', newHeight);
}
}

</script>
)";

        os << R"(
</head>
<body>
<div id="zoomPanel">
<button id="closeButton">close</button>
<div id="zoomTreeContainer"></div>
</div>

<div class="controls">
  <button id="zoomIn">+</button>
  <button id="zoomOut">-</button>
</div>

<div class="container">
<ol>
)";
    }
    else
{
    os << "<!DOCTYPE svg PUBLIC \" -//W3C//DTD SVG 1.0//EN\" \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n\n";
    }
}

void write_tree(const tree_info& tree, ostream& os, bool use_html)
{
    const vector<node>& nodes = tree.nodes;
    const Layout& l = tree.node_positions;

    // find bbox
    double min_offset = l.GetMinOffset();
    double max_offset = l.GetMaxOffset();
    double min_depth = l.GetMinDepth();
    double max_depth = l.GetMaxDepth();

    double normalizer = 1600.0 / (l.GetMaxOffset() - min_offset + 50);
    int gap = 20;
    double scale = 0.25;

    if (use_html)
    {
        os << "<li>" << endl;
    }


    os << "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" width=\"" << scale*(max_offset - min_offset) << "\" height=\"" << scale*(max_depth - min_depth) << "\" viewBox=\"" << min_offset - gap << " " << min_depth - gap << " " << max_offset + (2 * gap) << " " << max_depth + (2 * gap) << "\" preserveAspectRatio=\"xMinYMin meet\">" << endl << endl;
    os << "<g>" << endl;
    for (int j = 0; j < (int)nodes.size(); ++j)
    {
        int child0 = nodes[j].GetChild0();
        int child1 = nodes[j].GetChild1();

        if (child0 != -1)
        {
            os << "<line x1='" << l[j].GetOffset() << "' y1='" << l[j].GetDepth() << "' x2='" << l[child0].GetOffset() << "' y2='" << l[child0].GetDepth() << "' /> <!-- " << j << "-" << child0 << " -->\n";
        }

        if (child1 != -1)
        {
            os << "<line x1='" << l[j].GetOffset() << "' y1='" << l[j].GetDepth() << "' x2='" << l[child1].GetOffset() << "' y2='" << l[child1].GetDepth() << "' /> <!-- " << j << "-" << child1 << " -->\n";
        }
    }

    for (int i = 0; i < (int)l.size(); ++i)
    {
        double output_val = 1.0;
        if (i < nodes.size())
        {
            output_val = nodes[i].output;
        }

        bool is_pruned = output_val == 0;
        os << "<circle cx = '" << l[i].GetOffset() << "' cy='" << l[i].GetDepth() << "' r='13' " << (is_pruned ? " class='pruned' " : "") << " /> <!-- " << i << " -->\n";
    }
    os << "</g>" << endl;

    os << "</svg>" << endl;
    if (use_html)
    {
        os << "</li>" << endl;
    }
}

void write_footer(ostream& os, bool use_html)
{
    if (use_html)
    {
        os << "</ol>" << endl;
        os << "</body>" << endl;
        os << "</html>" << endl;
    }
}

bool ends_with(const string &str, const string& end)
{
    auto pos = str.rfind(end);
    return (pos != string::npos && pos == str.size() - end.size());
}

int main(int argc, char* argv[])
{
    // open file
    string input_filename = argv[1];
    string output_filename = argv[2];

    int max_steps = 10000;
    if (argc > 3)
    {
        max_steps = std::stoi(argv[3]);
    }
        
    bool fix_parents = false;
    bool SimpleLayout = false;
    if (argc > 4)
    {
        for (int i = 4; i < argc; i++)
        {
            if (strcmp(argv[i], "fix") == 0)
            {
                fix_parents = true;
            }
            if (strcmp(argv[i], "simple") == 0)
            {
                SimpleLayout = true;
            }
        }
    }

    bool bdt_format = ends_with(input_filename, ".ensemble");
    ifstream ifs(input_filename);

    // check that it opened
    if (ifs.bad())
    {
        cerr << "error opening file " << input_filename << endl;
        return 1;
    }

    ofstream ofs(output_filename);

    // check that it opened
    if (ofs.bad())
    {
        cerr << "error opening file " << output_filename << endl;
        return 1;
    }
    
    bool use_html = ends_with(output_filename, ".html");

    QpLayoutGenerator::Params p;
    p.depthSpace = 40.0;
    p.depthSpaceGrowthFactor = 1.04; // 1.08;
    p.offsetSpace = 40.0;
    p.offsetSpaceGrowthFactor = 10.0;
    p.gdNumSteps = max_steps;
    p.gd_learning_rate = 0.01;
    p.springRestLength = p.offsetSpace;
    p.spring_coeff_growth = 1; // 1.5;
    p.postprocess = fix_parents;
    p.SimpleLayout = SimpleLayout;

    try
    {
        if (bdt_format)
        {
            // read header line
            string dummy;
            getline(ifs, dummy);
        }
    }
    catch (runtime_error e)
    {
        cerr << "runtime error: " << e.what() << std::endl;
    }

    vector<tree_info> trees;
    while (true)
    {
        QpLayoutGenerator g(p);
        vector<node> nodes = read_nodes(ifs, bdt_format);
        if (nodes.size() == 0)
        {
            break;
        }

        vector<node> interior_nodes;
        if (bdt_format)
        {
            size_t num_interior_nodes = nodes.size() / 2;
            interior_nodes = vector<node>(nodes.begin(), nodes.begin() + num_interior_nodes);
        }
        else
        {
            interior_nodes = nodes;
        }

        auto l = g.generate(interior_nodes);
        trees.emplace_back(nodes, interior_nodes, l);
    }

    try
    {
        write_header(p, ofs, use_html);

        for (const auto& tree : trees)
        {
            write_tree(tree, ofs, use_html);
        }

        write_footer(ofs, use_html);

    }
    catch (runtime_error e)
    {
        cerr << "runtime error: " << e.what() << std::endl;
    }

    return 0;
}




