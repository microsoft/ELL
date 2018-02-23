////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelOptimizer.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "Model.h"
#include "ModelOptimizerOptions.h"
#include "ModelTransformer.h"

// utilities
#include "Exception.h"

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

    private:
        std::vector<std::unique_ptr<OptimizationPass>> _passes;
    };

    /// <summary> 
    /// A class for optimizing models. Contains a list of optimization passes to be run on the model. 
    /// Client code typically doesn't need to use this class -- it's created by the map compiler when compiling a map.
    /// </summary>
    class ModelOptimizer
    {
    public:
        /// <summary> Adds a pass to the list of passes the optimizer will run. </summary>
        ///
        /// <param name="pass"> The pass to add. </param>
        void AddPass(std::unique_ptr<OptimizationPass> pass);

        /// <summary> Optimize a model (by running the optimization passes that have been added to this optimizer). </summary>
        ///
        /// <param name="model"> The model to optimize. </param>
        ///
        /// <returns> A new, optimized, model. </returns>
        Model OptimizeModel(const Model& model);

        //
        // Internal routines  
        //

        /// <summary> Returns the port elements from the new model corresponding to the given port on the input model </summary>
        template <typename ValueType>
        PortElements<ValueType> GetCorrespondingOutputs(const OutputPort<ValueType>& port);

        /// <summary> Returns the port elements from the new model corresponding to the given port on the input model </summary>
        PortElementsBase GetCorrespondingOutputs(const OutputPortBase& port);

        /// <summary> Returns the port elements from the new model corresponding to the given elements on the input model </summary>
        template <typename ValueType>
        PortElements<ValueType> GetCorrespondingOutputs(const PortElements<ValueType>& elements);

        /// <summary> Returns the port elements from the new model corresponding to the given elements on the input model </summary>
        PortElementsBase GetCorrespondingOutputs(const PortElementsBase& elements);

        /// <summary> Returns the input node from the new model corresponding to the given input node on the input model </summary>
        template <typename ValueType>
        InputNode<ValueType>* GetCorrespondingInputNode(const InputNode<ValueType>* node);

        /// <summary> Returns the input node from the new model corresponding to the given input node on the input model </summary>
        InputNodeBase* GetCorrespondingInputNode(const InputNodeBase* node);

        ModelTransformer& GetTransformer();

    private:
        OptimizationPassList _passes;
        ModelTransformer _transformer;
    };

}
}
