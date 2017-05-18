////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SupervisedExampleBuilder.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Example.h"
#include "SparseEntryParser.h"

// utilities
#include "Parser.h"

// stl
#include <memory>
#include <string>

namespace ell
{
namespace data
{
    /// <summary> A supervised example builder. </summary>
    ///
    /// <typeparam name="VectorElementParserType"> Type of the vector element parser type. </typeparam>
    /// <typeparam name="DataVectorType"> Type of the default data vector type. </typeparam>
    template <typename VectorElementParserType, typename DataVectorType>
    class SupervisedExampleBuilder
    {
    public:
        /// <summary> Ctor. </summary>
        ///
        /// <param name="parser"> The parser. </param>
        /// <param name="hasWeight"> true if this object has weight. </param>
        SupervisedExampleBuilder(VectorElementParserType parser, bool hasWeight = false);

        /// <summary> Takes a std::string that represents a supervised example, and returns a
        /// SupervisedExample object. </summary>
        ///
        /// <param name="pExampleString"> The sp example string. </param>
        ///
        /// <returns> A SupervisedExample. </returns>
        AutoSupervisedExample Build(std::shared_ptr<const std::string> pExampleString);

    private:
        void HandleErrors(utilities::ParseResult result, const std::string& str);

        VectorElementParserType _instanceParser;
        bool _hasWeight;
    };
}
}

#include "../tcc/SupervisedExampleBuilder.tcc"
