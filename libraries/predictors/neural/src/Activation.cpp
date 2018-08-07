////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Activation.cpp (neural)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// activations
#include "TanhActivation.h"
#include "SigmoidActivation.h"
#include "HardSigmoidActivation.h"
#include "ReLUActivation.h"
#include "LeakyReLUActivation.h"
#include "ParametricReLUActivation.h"

namespace ell
{
namespace predictors
{
namespace neural
{

    template <typename ElementType>
    void Activation<ElementType>::LegacyReadFromArchive(utilities::Unarchiver& archiver)
    {
        // serialization compatibility (the old serialization format put the activation type in the typename of this layer).
        // so we need to fetch the type name from the archiver and create the appropriate activation object.
        auto type = archiver.GetCurrentObjectInfo().type;
        if (type.find("TanhActivation") != std::string::npos)
        {
            Reset(new TanhActivation<ElementType>());
        }
        else if (type.find("HardSigmoidActivation") != std::string::npos)
        {
            Reset(new HardSigmoidActivation<ElementType>());
        }
        else if (type.find("SigmoidActivation") != std::string::npos)
        {
            Reset(new SigmoidActivation<ElementType>());
        }
        else if (type.find("LeakyReLUActivation") != std::string::npos)
        {
            if (archiver.HasNextPropertyName("leakyFactor"))
            {
                ElementType leakyFactor;
                archiver["leakyFactor"] >> leakyFactor;
                Reset(new LeakyReLUActivation<ElementType>(leakyFactor));
            }
            else
            {
                Reset(new LeakyReLUActivation<ElementType>());
            }
        }
        else if (type.find("ParametricReLUActivation") != std::string::npos)
        {
            typename ParametricReLUActivation<ElementType>::TensorType alpha;
            math::TensorArchiver::Read(alpha, "alpha", archiver);
            Reset(new ParametricReLUActivation<ElementType>(alpha));
        }
        else if (type.find("ReLUActivation") != std::string::npos)
        {
            Reset(new ReLUActivation<ElementType>());
        }
        else
        {
            throw new utilities::DataFormatException(utilities::DataFormatErrors::badFormat, std::string("Unknown activation type: ") + type);
        }
    }

    // Explicit instantiations
    template class Activation<float>;
    template class Activation<double>;
}
}
}
