////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SetCompilerOptionsTransformation.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Transformation.h"

#include <utilities/include/PropertyBag.h>

#include <string>

namespace ell
{
namespace model
{
    class SetCompilerOptionsTransformation : public Transformation
    {
    public:
        /// <summary> Constructor from a PropertyBag containing model- and node-level options </summary>
        ///
        /// <param name="options"> A PropertyBag with optimizer options properties optionally set
        ///     for the model and some of its nodes.
        ///
        ///     The schema of the data in the property bag is:
        ///     ```
        ///     { 'model' : <options>,
        //        'nodes' : { <node-id> : <options>,
        ///                   <node-id> : <options>,
        ///        ...      }
        ///     }
        ///     ```
        ///     where `<options>` represents a `ModelOptimizerOptions` encoded as a `PropertyBag`.
        ///     Note that the 'model' entry, the 'nodes' entry, my either (or both) be absent.
        ///     If the 'nodes' entry is present, it need not contain all (or even any) settings
        ///     for the nodes in the model.
        /// </param>
        ///
        /// <returns>
        ///     Returns a transformation that will set the given options on the model it is applied to.
        ///     The result model will have a metadata property called 'optimizerOptions' that contains the 'model' options,
        ///     and each node named in the 'nodes' section will also have a 'optimizerOptions' metadata property containing
        ///     the options provided for that node.
        ///
        ///     If neither the 'model' or 'nodes' sections are present, the transformation just returns the input submodel.
        /// </returns>
        explicit SetCompilerOptionsTransformation(const utilities::PropertyBag& options);

        /// <summary> Transforms the submodel using the given transformer </summary>
        Submodel Transform(const Submodel& submodel, ModelTransformer& transformer, const TransformContext& context) const override;

        /// <summary> Returns the ID for this transformation </summary>
        std::string GetRuntimeTypeName() const override { return "SetModelOptimzerOptions"; }

    private:
        utilities::PropertyBag _options;
    };
} // namespace model
} // namespace ell
