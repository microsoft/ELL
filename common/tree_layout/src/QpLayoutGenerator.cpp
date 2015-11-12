// QpLayoutGenerator.cpp

#include "QpLayoutGenerator.h"
#include <cmath>

using std::move;

namespace tree_layout
{

	QpLayoutGenerator::Params::Params()
	{}

    QpLayoutGenerator::QpLayoutGenerator() : _params()
	{}

	QpLayoutGenerator::QpLayoutGenerator(Params p) : _params(p)
	{}

	QpLayoutGenerator::VertexInfo::VertexInfo(int i, double s) :
		index(i), space_left(s)
	{}

	void QpLayoutGenerator::Project()
	{
		for (int i = 1; i < (int)_layers.size(); ++i)
		{
			Project(i);
		}
	}

	void QpLayoutGenerator::Project(int layer_index)
	{
		// get current layer and its Size
		auto& layer = _layers[layer_index];
		int size = (int)layer.size();
		int first_unset = 0;	// index of the first coordinate in the layer that is not yet set

		// iterate until all offsets at this layer are set
		while (first_unset < size)
		{
			double shifted_sum = _offsets[layer[first_unset].index] - layer[first_unset].space_left;
			double min_shifted_mean = shifted_sum;
			int last = first_unset;

			for (int i = 1; first_unset + i < size; ++i)
			{
				shifted_sum += _offsets[layer[first_unset + i].index] - layer[first_unset + i].space_left;
				double shifted_mean = shifted_sum / (i+1);
				if (shifted_mean <= min_shifted_mean)
				{
					min_shifted_mean = shifted_mean;
					last = first_unset + i;
				}
			}

			for (int i = first_unset; i <= last; ++i)
			{
				_offsets[layer[i].index] = min_shifted_mean + layer[i].space_left;
			}

			first_unset = last + 1;
		}
	}

	Layout QpLayoutGenerator::GetLayout() const
	{
		int num_layers = (int)_layers.size();
		vector<double> depths(num_layers);
		depths[0] = 0.0;
		double depth_space = _params.depth_space * pow(_params.depth_space_growth_factor, num_layers - 1.0);

		// find min max offsets
		double min_offset = _offsets[0];
		double max_offset = min_offset;
		for (int i = 1; i < num_layers; ++i)
		{
			depths[i] = depths[i - 1] + depth_space;

			depth_space /= _params.depth_space_growth_factor;
			int first_in_layer = _layers[i][0].index;
			int last_in_layer = _layers[i][_layers[i].size() - 1].index;

			min_offset = fmin(min_offset, _offsets[first_in_layer]);
			max_offset = fmax(max_offset, _offsets[last_in_layer]);
		}

		// max depth
		double max_depth = depths[num_layers - 1];

		// create Layout
		Layout l(_offsets.size(), min_offset, max_offset, 0.0, max_depth);

		// fill Layout positions
		for (int i = 0; i < (int)l.size(); ++i)
		{
			l[i].SetDepth(depths[_depth_index[i]]);
			l[i].SetOffset(_offsets[i]);
		}

		return l;
	}
}