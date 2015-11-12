// QpLayoutGenerator.h

#pragma once

#include "Layout.h"
#include <vector>

using std::vector;

namespace tree_layout
{
	/// An algorithhm for laying out a tree based on a springs and constraints physical system, which is minimized with gradient descent
	///
	class QpLayoutGenerator
	{
	public:

		/// User configurable parameters of the Layout algorithm
		struct Params
		{
			Params();

			/// The maximum number of gradient descent steps to take. The more steps, the longer the running time, and the better the solution
			///
			int gd_num_steps = 5000;

			/// The gradient descent step Size
			///
			double gd_learning_rate = 0.01;

			/// The space between vertices at depth k and vertices at depth k+1 (if the tree is top down, this is the vertical spacing)
			///
			double depth_space = 1.0;

			/// The depth spacing closer to the root is larger. This is the multiplier that determines the spacing depth spacing at different levels
			///
			double depth_space_growth_factor = 1.04;

			/// The default space between adjacent vertices at the same depth (if the tree is top down, this is the default horizontal spacing)
			///
			double offset_space = 1.0;

			/// This parameters controls the space between adjacent vertices at the same depth that have a large tree distance 
			///
			double offset_space_growth_factor = 0.5;

			/// Springs at greater depth have higher spring coefficients, by pow(depth, spring_coeff_growth)
			///
			double spring_coeff_growth = 1.0;

			/// Natural rest length of springs connecting Children. 
			///
			double spring_rest_length = 0;

			/// If true, try to move parent nodes toward midpoint of Children at end
			bool postprocess = false;

			/// If true, use simple non-iterative Layout alg that scrunches trees as close as they can be without distorting them
			bool SimpleLayout = false;
		};

		/// Constructs a generator with default parameters
		///
		QpLayoutGenerator();

		/// Constructs a generator with user defined parameters
		///
		QpLayoutGenerator(Params p);

		/// Calculates the tree Layout
		///
		template<typename ChildrenVectorType>
		Layout generate(const ChildrenVectorType& Children);

	private:

		struct VertexInfo
		{
			VertexInfo(int i, double s);
			int index;
			double space_left;
		};

		template<typename ChildrenVectorType>
		void BuildLayers(const ChildrenVectorType& Children);

		template<typename ChildrenVectorType>
		void BuildLayers(const ChildrenVectorType& Children, int index, vector<int>& ancestors, vector<vector<int>>& prev_layer_ancestors);
		
		template<typename ChildrenVectorType>
		void Optimize(const ChildrenVectorType& Children);

		template<typename ChildrenVectorType>
		void GdStep(const ChildrenVectorType& Children, double step_size);

		template<typename ChildrenVectorType>
		void ComputeGradient(const ChildrenVectorType& Children, const vector<double>& offsets, const vector<int>& depths, double step_size, vector<double>& grad);

		template<typename ChildrenVectorType>
		void MoveParents(const ChildrenVectorType& Children, int node_index, double step_size);

		template<typename ChildrenVectorType>
		std::vector<std::pair<double, double>> SimpleLayout(const ChildrenVectorType& Children, int node_index, int depth);

		template<typename ChildrenVectorType>
		void IncrementOffsets(const ChildrenVectorType& Children, int node_index, double displacement);

		void Project();
		void Project(int layer_index);
		Layout GetLayout() const;

		vector<vector<VertexInfo>> _layers;
		vector<double> _offsets;
		vector<int> _depth_index;
		vector<double> _gd_increment;
		Params _params;
	};
}

#include "../tcc/QpLayoutGenerator.tcc"


