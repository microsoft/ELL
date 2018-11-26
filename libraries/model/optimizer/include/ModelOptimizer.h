////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelOptimizer.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/MapCompilerOptions.h>
#include <model/include/Model.h>
#include "ModelOptimizerOptions.h"
#include <model/include/ModelTransformer.h>

#include <utilities/include/Exception.h>

namespace ell
{
namespace model
{
    class OptimizationPass;

    /// <summary> A class used to hold the list of optimization passes included in an optimizer instance. </summary>
    class OptimizationPassList
    {
    public:
        ~OptimizationPassList();

        void AddPass(std::unique_ptr<OptimizationPass> pass);

        using InternalListType = std::vector<std::unique_ptr<OptimizationPass>>;
        InternalListType::iterator begin();
        InternalListType::iterator end();
        InternalListType::const_iterator begin() const;
        InternalListType::const_iterator end() const;

    private:
        std::vector<std::unique_ptr<OptimizationPass>> _passes;
    };

    class ModelOptimizer;

    class ModelOptimizerContext
    {
    public:
        ModelOptimizerContext() = default;
        ModelOptimizerContext(const ModelOptimizerContext& other) = delete;

        /// <summary> Gets the `ModelTransformer` being used for transforming the dataset during this invocation of the optimizer. </summary>
        ModelTransformer& GetTransformer();

        //
        // Internal routines
        //

        /// <summary> Returns the port elements from the new model corresponding to the given port on the input model </summary>
        const OutputPortBase& GetCorrespondingOutputs(const OutputPortBase& port);

        /// <summary> Returns the port elements from the new model corresponding to the given elements on the input model </summary>
        const OutputPortBase& GetCorrespondingOutputs(const PortElementsBase& elements);

        /// <summary> Returns the input node from the new model corresponding to the given input node on the input model </summary>
        InputNodeBase* GetCorrespondingInputNode(const InputNodeBase* node);

    private:
        ModelTransformer _transformer;
    };

    /// <summary>
    /// A class for optimizing models. Contains a list of optimization passes to be run on the model.
    /// Client code typically doesn't need to use this class -- it's created by the map compiler when compiling a map.
    /// </summary>
    class ModelOptimizer
    {
    public:
        ModelOptimizer(const MapCompilerOptions& settings);

        /// <summary> Adds a pass to the list of passes the optimizer will run. </summary>
        ///
        /// <param name="pass"> The pass to add. </param>
        void AddPass(std::unique_ptr<OptimizationPass> pass);

        /// <summary> Optimize a model (by running the optimization passes that have been added to this optimizer). </summary>
        ///
        /// <param name="model"> The model to optimize. </param>
        ///
        /// <returns> A new, optimized, model. </returns>
        Model OptimizeModel(const Model& model, ModelOptimizerContext& context) const;

        const MapCompilerOptions& GetSettings() const { return _settings; }

    private:
        OptimizationPassList _passes;
        MapCompilerOptions _settings;
    };
} // namespace model
} // namespace ell
