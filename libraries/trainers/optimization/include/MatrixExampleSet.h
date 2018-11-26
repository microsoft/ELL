////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MatrixExampleSet.h (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Common.h"
#include "IndexedContainer.h"
#include "OptimizationExample.h"

#include <math/include/Matrix.h>

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        /// <summary> An example set stored as a pair of matrices, one for input and the other for output. </summary>
        template <typename ElementType>
        class MatrixExampleSet : public IndexedContainer<Example<math::ConstRowVectorReference<ElementType>, math::ConstRowVectorReference<ElementType>>>
        {
        public:
            using ExampleType = Example<math::ConstRowVectorReference<ElementType>, math::ConstRowVectorReference<ElementType>>;

            /// <summary> Constructor. </summary>
            MatrixExampleSet(math::RowMatrix<ElementType> input, math::RowMatrix<ElementType> output);

            /// <summary> Returns the number of elements in the container </summary>
            size_t Size() const override { return _input.NumRows(); }

            /// <summary> Gets a reference to the elements that corresponds to a given index. </summary>
            ExampleType Get(size_t index) const override;

        private:
            math::RowMatrix<ElementType> _input;
            math::RowMatrix<ElementType> _output;
        };
    } // namespace optimization
} // namespace trainers
} // namespace ell

#pragma region implementation

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        template <typename ElementType>
        MatrixExampleSet<ElementType>::MatrixExampleSet(math::RowMatrix<ElementType> input, math::RowMatrix<ElementType> output) :
            _input(std::move(input)),
            _output(std::move(output))
        {
            if (_input.NumRows() != _output.NumRows())
            {
                throw OptimizationException("Number of inputs and outputs don't match");
            }
        }

        template <typename ElementType>
        auto MatrixExampleSet<ElementType>::Get(size_t index) const -> ExampleType
        {
            return ExampleType(_input.GetRow(index), _output.GetRow(index));
        }
    } // namespace optimization
} // namespace trainers
} // namespace ell

#pragma endregion implementation
