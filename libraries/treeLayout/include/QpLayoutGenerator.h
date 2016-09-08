////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     QpLayoutGenerator.h (treeLayout)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Layout.h"

// stl
#include <cstdint>
#include <vector>

namespace treeLayout
{
    /// <summary>
    /// An algorithhm for laying out a tree based on a springs and constraints physical system, which
    /// is minimized with gradient descent.
    /// </summary>
    class QpLayoutGenerator
    {
    public:
        /// <summary> User configurable parameters of the Layout algorithm. </summary>
        struct Parameters
        {
            Parameters() = default;

            /// <summary>
            /// The maximum number of gradient descent steps to take. The more steps, the longer the running
            /// time, and the better the solution.
            /// </summary>
            int gdNumSteps = 5000;

            /// <summary> The gradient descent step Size. </summary>
            double gd_learning_rate = 0.01;

            /// <summary>
            /// The space between vertices at depth k and vertices at depth k+1 (if the tree is top down,
            /// this is the vertical spacing)
            /// </summary>
            double depthSpace = 1.0;

            /// <summary>
            /// The depth spacing closer to the root is larger. This is the multiplier that determines the
            /// spacing depth spacing at different levels.
            /// </summary>
            double depthSpaceGrowthFactor = 1.04;

            /// <summary>
            /// The default space between adjacent vertices at the same depth (if the tree is top down, this
            /// is the default horizontal spacing)
            /// </summary>
            double offsetSpace = 1.0;

            /// <summary>
            /// This parameters controls the space between adjacent vertices at the same depth that have a
            /// large tree distance.
            /// </summary>
            double offsetSpaceGrowthFactor = 0.5;

            /// <summary>
            /// Springs at greater depth have higher spring coefficients, by pow(depth, spring_coeff_growth)
            /// </summary>
            double spring_coeff_growth = 1.0;

            /// <summary> Natural rest length of springs connecting Children. </summary>
            double springRestLength = 0;

            /// <summary> If true, try to move parent nodes toward midpoint of Children at end. </summary>
            bool postprocess = false;

            /// <summary>
            /// If true, use simple non-iterative Layout alg that scrunches trees as close as they can be
            /// without distorting them.
            /// </summary>
            bool SimpleLayout = false;
        };

        QpLayoutGenerator() = default;

        /// <summary> Constructs a layout generator with user defined parameters. </summary>
        ///
        /// <param name="parameters"> Layout generator parameters. </param>
        QpLayoutGenerator(const Parameters& parameters);

        /// <summary> Calculates the tree Layout. </summary>
        ///
        /// <typeparam name="ChildrenVectorType"> Type of the children vector. </typeparam>
        /// <param name="Children"> The children vector. </param>
        ///
        /// <returns> A Layout. </returns>
        template <typename ChildrenVectorType>
        Layout generate(const ChildrenVectorType& Children);

    private:
        struct VertexInfo
        {
            VertexInfo(uint64_t i, double s);

            uint64_t index;
            double space_left;
        };

        template <typename ChildrenVectorType>
        void BuildLayers(const ChildrenVectorType& Children);

        template <typename ChildrenVectorType>
        void BuildLayers(const ChildrenVectorType& Children, uint64_t index, std::vector<uint64_t>& ancestors, std::vector<std::vector<uint64_t>>& prev_layer_ancestors);

        template <typename ChildrenVectorType>
        void Optimize(const ChildrenVectorType& Children);

        template <typename ChildrenVectorType>
        void GdStep(const ChildrenVectorType& Children, double step_size);

        template <typename ChildrenVectorType>
        void ComputeGradient(const ChildrenVectorType& Children, const std::vector<double>& offsets, const std::vector<uint64_t>& depths, double step_size, std::vector<double>& grad);

        template <typename ChildrenVectorType>
        void MoveParents(const ChildrenVectorType& Children, uint64_t node_index, double step_size);

        template <typename ChildrenVectorType>
        std::vector<std::pair<double, double>> SimpleLayout(const ChildrenVectorType& Children, uint64_t node_index, uint64_t depth);

        template <typename ChildrenVectorType>
        void IncrementOffsets(const ChildrenVectorType& Children, uint64_t node_index, double displacement);

        void Project();

        void Project(uint64_t layer_index);

        Layout GetLayout() const;

        std::vector<std::vector<VertexInfo>> _layers;
        std::vector<double> _offsets;
        std::vector<uint64_t> _depth_index;
        std::vector<double> _gd_increment;
        Parameters _parameters;
    };
}

#include "../tcc/QpLayoutGenerator.tcc"
