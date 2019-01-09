////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Example.h (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <memory>

namespace ell
{
namespace optimization
{
    /// <summary> Represents an example for an Empirical Risk Minimization optimizer</summary>
    template <typename InputT, typename OutputT>
    struct Example
    {
        using InputType = InputT;
        using OutputType = OutputT;

        Example() = default;
        Example(const Example& other) = default;
        Example(Example&&) = default;

        Example& operator=(const Example&) = default;
        Example& operator=(Example&&) = default;

        /// <summary> Constructor. </summary>
        Example(InputType input, OutputType output, double weight = 1.0);

        /// <summary> Upcasting copy constructor. </summary>
        template <typename InputBase, typename OutputBase>
        Example(const Example<InputBase, OutputBase>& other);

        /// <summary> Upcasting move constructor. </summary>
        template <typename InputBase, typename OutputBase>
        Example(Example<InputBase, OutputBase>&& other);

        /// <summary> The input or instance. </summary>
        InputType input;

        /// <summary> The output or label. </summary>
        OutputType output;

        /// <summary> The example weight. </summary>
        double weight;
    };
} // namespace optimization
} // namespace ell

#pragma region implementation

namespace ell
{
namespace optimization
{
    template <typename InputT, typename OutputT>
    Example<InputT, OutputT>::Example(InputType input, OutputType output, double weight) :
        input(std::move(input)),
        output(std::move(output)),
        weight(weight)
    {
    }

    template <typename InputT, typename OutputT>
    template <typename InputBase, typename OutputBase>
    Example<InputT, OutputT>::Example(const Example<InputBase, OutputBase>& other) :
        input(other.input),
        output(other.output),
        weight(other.weight)
    {
    }

    template <typename InputT, typename OutputT>
    template <typename InputBase, typename OutputBase>
    Example<InputT, OutputT>::Example(Example<InputBase, OutputBase>&& other) :
        input(std::move(other.input)),
        output(std::move(other.output)),
        weight(other.weight)
    {
    }
} // namespace optimization
} // namespace ell

#pragma endregion implementation
