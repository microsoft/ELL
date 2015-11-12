// QpLayoutGenerator.tcc

#include <stdexcept>
#include <cassert>
#include <iostream>
#include <numeric>
#include <algorithm>

using namespace std;

namespace tree_layout
{
    template<typename ChildrenVectorType>
    Layout QpLayoutGenerator::generate(const ChildrenVectorType& Children)
    {
        size_t num_nodes = Children.size() * 2 + 1;
        // initialize memory
        _offsets.resize(num_nodes);
        _depth_index.resize(num_nodes);
        _gd_increment.resize(num_nodes);
        BuildLayers(Children);

        if (_params.SimpleLayout)
        {
            SimpleLayout(Children, 0, 0);
        }
        else
        {
            // initialize offsets
            Project();

            // run the optimization
            Optimize(Children);
            if (_params.postprocess)
            {
                for (int i = 0; i < 10; i++)
                {
                    MoveParents(Children, 0, 0.1);
                    Project();
                }
            }
        }

        // create the Layout object
        auto l = GetLayout();

        // clear memory
        _layers.clear();
        _offsets.clear();
        _depth_index.clear();

        // return the result
        return l;
    }

    template<typename ChildrenVectorType>
    void QpLayoutGenerator::BuildLayers(const ChildrenVectorType& Children)
    {
        vector<int> ancestors;
        vector<vector<int>> prev_layer_ancestors;
        BuildLayers(Children, 0, ancestors, prev_layer_ancestors);
    }

    template<typename ChildrenVectorType>
    void QpLayoutGenerator::BuildLayers(const ChildrenVectorType& Children, int index, vector<int>& ancestors, vector<vector<int>>& prev_layer_ancestors)
    {
        int depth = (int)ancestors.size();
        if (depth > (int)Children.size())
        {
            throw runtime_error("infinite recursion: perhaps the Children structure is loopy");
        }

        // record the depth
        _depth_index[index] = depth;

        // make sure that the _layers vector is long enough
        if (_layers.size() <= depth)
        {
            _layers.resize(depth + 1);
        }

        // get the current layer and its Size
        auto& layer = _layers[depth];
        int size = (int)layer.size();

        int closest_common_ancestor = 0;
        if (prev_layer_ancestors.size() > depth && prev_layer_ancestors[depth].size() > depth - 1)
        {
            // count down until we reach the root or our ancestor list diverges from the one stored in prev_layer_ancestors
            for (int d = depth - 1; d >= 0 && ancestors[d] != prev_layer_ancestors[depth][d]; --d)
            {
                ++closest_common_ancestor;
            }
        }
        double space_left = 1.0 + closest_common_ancestor;

        // get the cumulative space_left in the layer
        double cum_space_left = _params.offset_space + _params.offset_space_growth_factor * log2(space_left);
        if (size > 0)
        {
            cum_space_left += layer[layer.size() - 1].space_left;
        }

        // Add the new vertex to _layers
        layer.emplace_back(index, cum_space_left);

        // store our ancestor list at our depth
        if (prev_layer_ancestors.size() <= depth)
        {
            prev_layer_ancestors.resize(depth + 1);
        }
        prev_layer_ancestors[depth] = ancestors;

        // termination condition of the recursion
        if (index < (int)Children.size())
        {
            ancestors.push_back(index);

            // recurse
            int child0 = Children[index].GetChild0();
            BuildLayers(Children, child0, ancestors, prev_layer_ancestors);

            int child1 = Children[index].GetChild1();
            BuildLayers(Children, child1, ancestors, prev_layer_ancestors);
            ancestors.pop_back();
        }
    }

    template<typename ChildrenVectorType>
    vector<pair<double, double>> QpLayoutGenerator::SimpleLayout(const ChildrenVectorType& Children, int node_index, int depth)
    {
        if (node_index >= Children.size()) // leaf node
        {
            return vector<pair<double, double>>();
        }

        // recurse
        int child0 = Children[node_index].GetChild0();
        int child1 = Children[node_index].GetChild1();

        vector<pair<double, double>> x0 = SimpleLayout(Children, child0, depth + 1); 
        vector<pair<double, double>> x1 = SimpleLayout(Children, child1, depth + 1); 
        size_t min_depth = min(x0.size(), x1.size());
        size_t max_depth = max(x0.size(), x1.size());

        double max_dist = _params.offset_space;
        for (int d = 0; d < min_depth; d++)
        {
            double gap = _params.offset_space +_params.offset_space_growth_factor * log2(2.0+d);
            double dist = gap + x0[d].second - x1[d].first;
            max_dist = max(dist, max_dist);
        }

        vector<pair<double, double>> result(max_depth+1);
        result[0] = make_pair(-max_dist / 2, max_dist / 2);
        for (int d = 0; d < max_depth; d++)
        {
            if (d < x0.size() && d < x1.size()) // we have both left and right subtree at this depth
            {
                result[d + 1] = make_pair(x0[d].first - max_dist / 2, x1[d].second + max_dist / 2);
            }
            else if (d >= x0.size()) // only right subtree
            {
                result[d + 1] = make_pair(x1[d].first + max_dist / 2, x1[d].second + max_dist / 2);
            }
            else // only left subtree
            {
                result[d + 1] = make_pair(x0[d].first - max_dist / 2, x0[d].second - max_dist / 2);
            }
        }
        IncrementOffsets(Children, child0, -max_dist / 2.0);
        IncrementOffsets(Children, child1, max_dist / 2.0);

        return result;
    }

    template<typename ChildrenVectorType>
    void QpLayoutGenerator::IncrementOffsets(const ChildrenVectorType& Children, int node_index, double displacement)
    {
        _offsets[node_index] += displacement;

        if (node_index < Children.size()) // leaf node
        {
            int child0 = Children[node_index].GetChild0();
            int child1 = Children[node_index].GetChild1();
            IncrementOffsets(Children, child0, displacement);
            IncrementOffsets(Children, child1, displacement);
        }
    }

    template<typename ChildrenVectorType>
    void QpLayoutGenerator::Optimize(const ChildrenVectorType& Children)
    {
        for (int t = 1; t <= _params.gd_num_steps; ++t)
        {
            vector<double> old_offsets = _offsets;
            GdStep(Children, _params.gd_learning_rate);
            Project();

            double total_disp = 0;
            for (int index = 0; index < _offsets.size(); index++)
            {
                double diff = old_offsets[index] - _offsets[index];
                total_disp += diff*diff;
            }
        }
    }

    template<typename ChildrenVectorType>
    void QpLayoutGenerator::GdStep(const ChildrenVectorType& Children, double step_size)
    {
        // initialize memory
        _gd_increment.assign(_gd_increment.size(), 0.0);
        ComputeGradient(Children, _offsets, _depth_index, step_size, _gd_increment);

        for (int i = 0; i < (int)_gd_increment.size(); ++i)
        {
            _offsets[i] += _gd_increment[i];
        }
    }

    template<typename ChildrenVectorType>
    void QpLayoutGenerator::ComputeGradient(const ChildrenVectorType& Children, const vector<double>& offsets, const vector<int>& depths, double step_size, vector<double>& grad)
    {
        size_t num_nodes = Children.size() * 2 + 1;

        for (int i = 0; i < Children.size(); ++i)
        {
            int child0 = Children[i].GetChild0();
            int child1 = Children[i].GetChild1();

            double parent_offset = offsets[i];
            double child0_offset = offsets[child0];
            double child1_offset = offsets[child1];

            double max_depth = (double)_layers.size();
            double spring_coeff = (1.0 + pow(depths[i], _params.spring_coeff_growth));

            double spring0_len = (parent_offset - child0_offset);
            double spring1_len = (parent_offset - child1_offset);
            double spring0_f = spring_coeff * (spring0_len - _params.spring_rest_length / 2.0);
            double spring1_f = spring_coeff * (spring1_len + _params.spring_rest_length / 2.0);

            // gradient step of parent towards Children
            grad[i] -= step_size * (spring0_f + spring1_f);

            // gradient step of Children towards parent
            grad[child0] += spring0_f * step_size;
            grad[child1] += spring1_f * step_size;
        }
    }

    template<typename ChildrenVectorType>
    void QpLayoutGenerator::MoveParents(const ChildrenVectorType& Children, int node_index, double step_size)
    {
        if (node_index < Children.size())
        {
            int child0 = Children[node_index].GetChild0();
            int child1 = Children[node_index].GetChild1();

            // recurse
            MoveParents(Children, child0, step_size);
            MoveParents(Children, child1, step_size);

            double parent_offset = _offsets[node_index];
            double child0_offset = _offsets[child0];
            double child1_offset = _offsets[child1];

            double midpt = (child0_offset + child1_offset) / 2.0;
            // move parent toward midpoint of Children
//            _offsets[node_index] = ((1.0 - step_size) * parent_offset) + (step_size * midpt);
            _offsets[node_index] += step_size*(midpt - parent_offset); // equivalent to above
        }
    }
}