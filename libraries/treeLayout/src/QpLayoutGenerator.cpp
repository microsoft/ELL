////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     QpLayoutGenerator.cpp (treeLayout)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "QpLayoutGenerator.h"

namespace ell
{
namespace treeLayout
{
    QpLayoutGenerator::QpLayoutGenerator(const Parameters& parameters)
        : _parameters(parameters)
    {
    }

    QpLayoutGenerator::VertexInfo::VertexInfo(size_t i, double s)
        : index(i), space_left(s)
    {
    }

    void QpLayoutGenerator::Project()
    {
        for (size_t i = 1; i < (size_t)_layers.size(); ++i)
        {
            Project(i);
        }
    }

    void QpLayoutGenerator::Project(size_t layer_index)
    {
        // get current layer and its Size
        auto& layer = _layers[layer_index];
        size_t size = (size_t)layer.size();
        size_t first_unset = 0; // index of the first coordinate in the layer that is not yet set

        // iterate until all offsets at this layer are set
        while (first_unset < size)
        {
            double shifted_sum = _offsets[layer[first_unset].index] - layer[first_unset].space_left;
            double min_shifted_mean = shifted_sum;
            size_t last = first_unset;

            for (size_t i = 1; first_unset + i < size; ++i)
            {
                shifted_sum += _offsets[layer[first_unset + i].index] - layer[first_unset + i].space_left;
                double shifted_mean = shifted_sum / (i + 1);
                if (shifted_mean <= min_shifted_mean)
                {
                    min_shifted_mean = shifted_mean;
                    last = first_unset + i;
                }
            }

            for (size_t i = first_unset; i <= last; ++i)
            {
                _offsets[layer[i].index] = min_shifted_mean + layer[i].space_left;
            }

            first_unset = last + 1;
        }
    }

    Layout QpLayoutGenerator::GetLayout() const
    {
        size_t numLayers = (size_t)_layers.size();
        std::vector<double> depths(numLayers);
        depths[0] = 0.0;
        double depthSpace = _parameters.depthSpace * pow(_parameters.depthSpaceGrowthFactor, numLayers - 1.0);

        // find min max offsets
        double min_offset = _offsets[0];
        double max_offset = min_offset;
        for (size_t i = 1; i < numLayers; ++i)
        {
            depths[i] = depths[i - 1] + depthSpace;

            depthSpace /= _parameters.depthSpaceGrowthFactor;
            size_t first_in_layer = _layers[i][0].index;
            size_t last_in_layer = _layers[i][_layers[i].size() - 1].index;

            min_offset = fmin(min_offset, _offsets[first_in_layer]);
            max_offset = fmax(max_offset, _offsets[last_in_layer]);
        }

        // max depth
        double max_depth = depths[numLayers - 1];

        // create Layout
        Layout l(_offsets.size(), min_offset, max_offset, 0.0, max_depth);

        // fill Layout positions
        for (size_t i = 0; i < (size_t)l.Size(); ++i)
        {
            l[i].SetDepth(depths[_depth_index[i]]);
            l[i].SetOffset(_offsets[i]);
        }

        return l;
    }
}
}
