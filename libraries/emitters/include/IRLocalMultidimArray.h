////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLocalMultidimArray.h (emitters)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IRLocalScalar.h"

// utilities
#include "Exception.h"
#include "IntegerNArray.h"

// stl
#include <algorithm>
#include <array>
#include <initializer_list>

namespace ell
{
namespace emitters
{
    /// <summary>
    /// Helper type for llvm values representing N-D arrays local to a function
    /// </summary>
    struct IRLocalMultidimArray
    {
    protected:
        /// <summary>
        /// Helper type for llvm values representing values within arrays local to a function
        /// </summary>
        struct IRLocalArrayElement
        {
            IRLocalArrayElement(IRFunctionEmitter& function, llvm::Value* data, llvm::Value* offset);

            IRLocalArrayElement& operator=(const IRLocalArrayElement& other);

            IRLocalArrayElement& operator=(llvm::Value* value);

            operator IRLocalScalar() const;

            IRFunctionEmitter& _function;
            llvm::Value* _data;
            llvm::Value* _offset;
        };

    public:
        /// <summary> Constructor from a pointer to data and a list of dimensions. </summary>
        ///
        /// <param name="function"> The current function being emitted. </param>
        /// <param name="data"> The pointer to the LLVM array to wrap. </param>
        /// <param name="extents"> The sizes of the array's dimensions. </param>
        IRLocalMultidimArray(IRFunctionEmitter& function, llvm::Value* data, std::vector<int> extents);

        /// <summary> Constructor from a pointer to data and a list of logical and physical dimensions. </summary>
        ///
        /// <param name="function"> The current function being emitted. </param>
        /// <param name="data"> The pointer to the LLVM array to wrap. </param>
        /// <param name="extents"> The sizes of the array's logical dimensions. </param>
        /// <param name="memorySize"> The sizes of the array's physical dimensions. </param>
        IRLocalMultidimArray(IRFunctionEmitter& function, llvm::Value* data, std::vector<int> extents, std::vector<int> memorySize);

        /// <summary> Indexing operator to return a reference to the specified element </summary>
        ///
        /// <param name="indices"> The indices of the element. </param>
        ///
        /// <return> An instance of IRLocalMultidimArray::IRLocalArrayElement to represent the value at the offset within the array </returns>
        IRLocalArrayElement operator()(std::vector<IRLocalScalar> indices) const;

        /// <summary> Indexing operator to return a reference to the specified element </summary>
        ///
        /// <param name="indices"> The indices of the element. </param>
        ///
        /// <return> An instance of IRLocalMultidimArray::IRLocalArrayElement to represent the value at the offset within the array </returns>
        IRLocalArrayElement operator()(std::vector<int> indices) const;

        /// <summary> Returns a LLVM pointer to the specified element </summary>
        ///
        /// <param name="indices"> The indices of the element. </param>
        ///
        /// <return> An instance of IRLocalPointer to represent the pointer at the offset within the array </returns>
        IRLocalPointer PointerTo(std::vector<IRLocalScalar> indices) const;

        /// <summary> Returns a LLVM pointer to the specified element </summary>
        ///
        /// <param name="indices"> The indices of the element. </param>
        ///
        /// <return> An instance of IRLocalPointer to represent the pointer at the offset within the array </returns>
        IRLocalPointer PointerTo(std::vector<int> indices) const;

        /// <summary> The function this value is in scope for. </summary>
        IRFunctionEmitter& function;

        /// <summary> The llvm::Value* being wrapped. </summary>
        llvm::Value* data = nullptr;

        std::vector<int> extents;
        std::vector<int> strides;
    };

    /// <summary>
    /// Helper type for llvm values representing N-D arrays local to a function, where order of dimensions can be represented logically
    /// </summary>
    template <size_t N>
    struct IRLocalNDimArray : IRLocalMultidimArray
    {
        IRLocalNDimArray(IRFunctionEmitter& function, llvm::Value* data, std::array<int, N> extents, std::array<int, N> layout)
            : IRLocalMultidimArray(function, data, ToLayoutOrder(extents, layout)), layout(layout)
        {
            constexpr auto expectedLayout = utilities::MakeNArray<N>();

            if (!std::is_permutation(layout.begin(), layout.end(), expectedLayout.begin(), expectedLayout.end()))
            {
                throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
            }
        }

        /// <summary> Indexing operator to return a reference to the specified element </summary>
        ///
        /// <param name="index"> The index of the element. </param>
        ///
        /// <return> An instance of IRLocalMultidimArray::IRLocalArrayElement to represent the value at the offset within the array </returns>
        IRLocalArrayElement operator()(std::array<IRLocalScalar, N> index) const
        {
            return IRLocalMultidimArray::operator()(ToLayoutOrder(index, layout));
        }

        /// <summary> Indexing operator to return a reference to the specified element </summary>
        ///
        /// <param name="index"> The index of the element. </param>
        ///
        /// <return> An instance of IRLocalMultidimArray::IRLocalArrayElement to represent the value at the offset within the array </returns>
        IRLocalArrayElement operator()(std::array<int, N> index) const
        {
            return IRLocalMultidimArray::operator()(ToLayoutOrder(index, layout));
        }

        std::array<int, N> layout;

    private:
        template <typename T>
        static constexpr auto ToLayoutOrder(const std::array<T, N>& array, const std::array<int, N>& layout)
        {
            return ToLayoutOrder(array, layout, std::make_index_sequence<N>{});
        }

        template <typename T, size_t... I>
        static constexpr auto ToLayoutOrder(const std::array<T, N>& array, const std::array<int, N>& layout, std::index_sequence<I...>)
        {
            return std::vector<T>{ array[layout[I]]... };
        }
    };

    /// <summary> Helper type for llvm values representing a matrix local to a function </summary>
    using IRLocalMatrix = IRLocalNDimArray<2>;

    /// <summary> Helper type for llvm values representing a tensor local to a function </summary>
    using IRLocalTensor = IRLocalNDimArray<3>;

    /// <summary> Represents row-major layout for use with IRLocalMatrix </summary>
    constexpr std::array<int, 2> RowMajorMatrixLayout{ 0, 1 };

    /// <summary> Represents column-major layout for use with IRLocalMatrix </summary>
    constexpr std::array<int, 2> ColumnMajorMatrixLayout{ 1, 0 };

    /// <summary> Represents row-major layout for use with IRLocalTensor </summary>
    constexpr std::array<int, 3> RowMajorTensorLayout{ 0, 1, 2 };

    /// <summary> Represents channel-major layout for use with IRLocalTensor </summary>
    constexpr std::array<int, 3> ChannelMajorTensorLayout{ 2, 1, 0 };
}
}