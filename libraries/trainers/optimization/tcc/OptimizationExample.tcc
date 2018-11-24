////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Example.tcc (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace ell
{
namespace trainers
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
    } // namespace optimization
} // namespace trainers
} // namespace ell
