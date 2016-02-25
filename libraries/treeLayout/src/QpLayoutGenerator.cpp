////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     QpLayoutGenerator.cpp (treeLayout)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "QpLayoutGenerator.h"

namespace treeLayout
{

    QpLayoutGenerator::Params::Params()
    {}

    QpLayoutGenerator::QpLayoutGenerator() : _params()
    {}

    QpLayoutGenerator::QpLayoutGenerator(Params p) : _params(p)
    {}

    QpLayoutGenerator::VertexInfo::VertexInfo(uint64 i, double s) :
        index(i), space_left(s)
    {}

    void QpLayoutGenerator::Project()
    {
        for (uint64 i = 1; i < (uint64)_layers.size(); ++i)
        {
            Project(i);
        }
    }

    void QpLayoutGenerator::Project(uint64 layer_index)
    {
        // get current layer and its Size
        auto& layer = _layers[layer_index];
        uint64 size = (uint64)layer.size();
        uint64 first_unset = 0;    // index of the first coordinate in the layer that is not yet set

        // iterate until all offsets at this layer are set
        while (first_unset < size)
        {
            double shifted_sum = _offsets[layer[first_unset].index] - layer[first_unset].space_left;
            double min_shifted_mean = shifted_sum;
            uint64 last = first_unset;

            for (uint64 i = 1; first_unset + i < size; ++i)
            {
                shifted_sum += _offsets[layer[first_unset + i].index] - layer[first_unset + i].space_left;
                double shifted_mean = shifted_sum / (i+1);
                if (shifted_mean <= min_shifted_mean)
                {
                    min_shifted_mean = shifted_mean;
                    last = first_unset + i;
                }
            }

            for (uint64 i = first_unset; i <= last; ++i)
            {
                _offsets[layer[i].index] = min_shifted_mean + layer[i].space_left;
            }

            first_unset = last + 1;
        }
    }

    Layout QpLayoutGenerator::GetLayout() const
    {
        uint64 numLayers = (uint64)_layers.size();
        std::vector<double> depths(numLayers);
        depths[0] = 0.0;
        double depthSpace = _params.depthSpace * pow(_params.depthSpaceGrowthFactor, numLayers - 1.0);

        // find min max offsets
        double min_offset = _offsets[0];
        double max_offset = min_offset;
        for (uint64 i = 1; i < numLayers; ++i)
        {
            depths[i] = depths[i - 1] + depthSpace;

            depthSpace /= _params.depthSpaceGrowthFactor;
            uint64 first_in_layer = _layers[i][0].index;
            uint64 last_in_layer = _layers[i][_layers[i].size() - 1].index;

            min_offset = fmin(min_offset, _offsets[first_in_layer]);
            max_offset = fmax(max_offset, _offsets[last_in_layer]);
        }

        // max depth
        double max_depth = depths[numLayers - 1];

        // create Layout
        Layout l(_offsets.size(), min_offset, max_offset, 0.0, max_depth);

        // fill Layout positions
        for (uint64 i = 0; i < (uint64)l.Size(); ++i)
        {
            l[i].SetDepth(depths[_depth_index[i]]);
            l[i].SetOffset(_offsets[i]);
        }

        return l;
    }
}
