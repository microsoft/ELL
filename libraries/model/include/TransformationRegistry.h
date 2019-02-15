////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TransformationRegistry.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Transformation.h"

#include <memory>
#include <vector>

namespace ell
{
namespace model
{
    /// <summary> A class for holding a set of Transformations to be used while optimizing a model </summary>
    class TransformationRegistry
    {
    public:
        template <typename TransformationType>
        void AddTransformation();

        template <typename TransformationType>
        void AddTransformation(const TransformationType& transformation);

        auto begin() const { return _transformations.cbegin(); }
        auto end() const { return _transformations.cend(); }

        static TransformationRegistry& GetGlobalRegistry();

    protected:
        std::vector<std::unique_ptr<Transformation>> _transformations;
    };

} // namespace model
} // namespace ell

#pragma region implementation

namespace ell
{
namespace model
{
    template <typename TransformationType>
    void TransformationRegistry::AddTransformation()
    {
        _transformations.push_back(std::make_unique<TransformationType>());
    }

    template <typename TransformationType>
    void TransformationRegistry::AddTransformation(const TransformationType& transformation)
    {
        _transformations.push_back(std::make_unique<TransformationType>(transformation));
    }
} // namespace model
} // namespace ell

#pragma endregion implementation
