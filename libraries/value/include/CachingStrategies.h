////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CachingStrategies.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CachingProvider.h"

namespace ell
{
namespace value
{
    enum class BoundaryConditionHandling : int
    {
        ZeroPadding
    };

    using ReduceFunctionType = void(value::Scalar, value::Scalar);
    void CopyReduce(value::Scalar, value::Scalar);
    void SumReduce(value::Scalar, value::Scalar);

    class CopyInputCopyOutput : public CachingProvider
    {
        void HandleCachingImpl(LoopNest&) override;
    };

    class CopyInputNoOutput : public CachingProvider
    {
        void HandleCachingImpl(LoopNest&) override;
    };

    class ZeroInputReduceOutput : public CachingProvider
    {
        void HandleCachingImpl(LoopNest&) override;
    };

    class BLASTCopy : public CachingProvider
    {
    public:
        void HandleCachingImpl(LoopNest&) override;

        Value _rawCache;
    };

    class GeneralCachingStrategy : public CachingProvider
    {
    public:
        void HandleCachingImpl(LoopNest&) override;
        Value _rawCache;
    };

    using SubMatrixCopyInCopyOutCache = CachingStrategyType<CopyInputCopyOutput>;
    using SubMatrixCopyIn = CachingStrategyType<CopyInputNoOutput>;
    using ZeroInputCopyOutMatrixCache = CachingStrategyType<ZeroInputReduceOutput>;

    using BLASTCopyCache = CachingStrategyType<BLASTCopy>;

} // namespace value
} // namespace ell
