// main.cpp

#include "IPrintable.h"
#include "PrintableMap.h"

// utilities
#include "JsonSerializer.h"
#include "files.h"
using utilities::OpenIfstream;

// layers
#include "Map.h"
using layers::Map;

#include<iostream>
using std::cerr;
using std::cout;
using std::endl;

#include <fstream>
using std::ifstream;

#include <stdexcept>
using std::runtime_error;

#include <memory>
using std::dynamic_pointer_cast;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cerr << "usage: print.exe <layers file>\n";
        return 1;
    }

    try
    {
        // open file
        auto layersFile = argv[1];
        ifstream layersFStream = OpenIfstream(layersFile);
        auto map = Map::Deserialize<PrintableMap>(layersFStream);
        // map->Print(cout);

        cout << R"aw(
<html>
<body>

<style>
    text
    {
        font-family:    sans-serif;
    }

                            rect
    {
        stroke:        #222222;
        stroke-width:    2;
    }
    
    rect.Interior
    {
        fill:        white;
    }

                            rect.ZeroLayer
    {
        fill:        #22AA22;
    }

                            rect.ScaleLayer
    {
        fill:        #FF2244;
    }

                            ellipse.Connector
    {
        fill:        #222222;
    }

                path
    {
        stroke:        #110011;
        stroke-width:    2;    
        fill:        none;
        stroke-dasharray:    4,2;
    }

                        </style>

                        <svg width="800" height="300">

                            <defs>
        <g id="Node">
            <ellipse class="Connector" cx="25" cy="0" rx="5" ry="5" />
            <ellipse class="Connector" cx="25" cy="40" rx="5" ry="5" />
            <rect class="Interior" x="0" y="0" rx="10" ry="10" width="50" height="40"/>
        </g>
    </defs>

                            <rect class="ZeroLayer" x="20" y="20" rx="10" ry="10" width="760" height="60"/>
        <text x="35" y="55" text-anchor="middle" fill="white" font-size="15" font-weight="bold">1</text>
        <text x="65" y="50" text-anchor="middle" fill="white" font-size="15" font-weight="bold" transform="rotate(-90,65,50)">INPUT</text>

                        <use xlink : href = "#Node" x = "80" y = "30" rx = "10" ry = "10"/>
            <text x = "105" y = "55" text-anchor = "middle">457.8</text>

                        <use xlink : href = "#Node" x = "135" y = "30" rx = "10" ry = "10"/>
            <text x = "160" y = "55" text-anchor = "middle">-12</text>

                        <use xlink : href = "#Node" x = "190" y = "30" rx = "10" ry = "10"/>

                        <use xlink : href = "#Node" x = "245" y = "30" rx = "10" ry = "10"/>

                        <use xlink : href = "#Node" x = "300" y = "30" rx = "10" ry = "10"/>

                        <rect class = "ScaleLayer" x = "20" y = "100" rx = "10" ry = "10" width = "760" height = "60"/>
            <text x = "35" y = "135" text-anchor = "middle" fill = "white" font-size = "15" font-weight = "bold">2</text>
            <text x = "65" y = "130" text-anchor = "middle" fill = "white" font-size = "15" font-weight = "bold" transform = "rotate(-90,65,130)">SUM</text>

                        <use xlink : href = "#Node" x = "80" y = "110" rx = "10" ry = "10"/>
            <text x = "105" y = "55" text-anchor = "middle">457.8</text>

                        <use xlink : href = "#Node" x = "135" y = "110" rx = "10" ry = "10"/>
            <text x = "160" y = "55" text-anchor = "middle">-12</text>

                        <use xlink : href = "#Node" x = "190" y = "110" rx = "10" ry = "10"/>

                        <use xlink : href = "#Node" x = "245" y = "110" rx = "10" ry = "10"/>

                        <use xlink : href = "#Node" x = "300" y = "110" rx = "10" ry = "10"/>

            <path d="M 105 75 q 10 15 20 15 h 15 q 10 0 20 15"/>
            <path d="M 105 75 q 10 15 20 15 h 70 q 10 0 20 15"/>

        </svg>
</html>
</body>
)aw";

    }
    catch (runtime_error e)
    {
        cerr << "runtime error: " << e.what() << std::endl;
    }

    // the end
    return 0;
}
