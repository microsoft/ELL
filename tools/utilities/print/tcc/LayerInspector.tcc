#pragma once

// model
#include "InputPort.h"
#include "Model.h"
#include "NeuralNetworkPredictorNode.h"
#include "Node.h"
#include "BinaryConvolutionalLayer.h"
#include "ConvolutionalLayer.h"
#include "PoolingLayer.h"
#include "ScalingLayer.h"

// stl
#include <vector>
#include <memory>

namespace ell
{
    static std::string PaddingSchemeToString(ell::predictors::neural::PaddingScheme scheme) {
        switch (scheme)
        {
        case ell::predictors::neural::PaddingScheme::zeros:
            return "zeros";
        case ell::predictors::neural::PaddingScheme::minusOnes:
            return "minusOnes";
        case ell::predictors::neural::PaddingScheme::alternatingZeroAndOnes:
            return "alternatingZeroAndOnes";
        case ell::predictors::neural::PaddingScheme::randomZeroAndOnes:
            return "randomZeroAndOnes";
        case ell::predictors::neural::PaddingScheme::min:
            return "min";
        case ell::predictors::neural::PaddingScheme::max:
            return "max";
        }
        return "";
    }

    static std::string ConvolutionMethodToString(ell::predictors::neural::ConvolutionMethod method) 
    {
        switch(method) {
            /// <summary> Normal method of doing convolution via reshaping input into columns and performing a gemm operation. </summary>
        case ell::predictors::neural::ConvolutionMethod::unrolled:
            return "unrolled";
        case ell::predictors::neural::ConvolutionMethod::diagonal:
            return "diagonal";
        case ell::predictors::neural::ConvolutionMethod::simple:
            return "simple";
        };
        return "";
    }
	static std::string BinaryConvolutionMethodToString(ell::predictors::neural::BinaryConvolutionMethod method)
	{
		switch (method) {
			/// <summary> Normal method of doing convolution via reshaping input into columns and performing a gemm operation. </summary>
		case ell::predictors::neural::BinaryConvolutionMethod::gemm:
			return "gemm";
		case ell::predictors::neural::BinaryConvolutionMethod::bitwise:
			return "bitwise";
		};
		return "";
	}
	static std::string BinaryWeightsScaleToString(ell::predictors::neural::BinaryWeightsScale method)
	{
		switch (method) {
			/// <summary> Normal method of doing convolution via reshaping input into columns and performing a gemm operation. </summary>
		case ell::predictors::neural::BinaryWeightsScale::none:
			return "none";
		case ell::predictors::neural::BinaryWeightsScale::mean:
			return "mean";
		};
		return "";
	}


    template<typename ElementType>
    std::vector<NameValue> InspectBinaryConvolutionalLayerParameters(const ell::predictors::neural::BinaryConvolutionalLayer<ElementType>* layer)
    {
        std::vector<NameValue> result;
        auto params = layer->GetConvolutionalParameters();
        result.push_back(NameValue{ "stride", std::to_string(params.stride) });
		result.push_back(NameValue{ "method", BinaryConvolutionMethodToString(params.method) });
		result.push_back(NameValue{ "receptiveField", std::to_string(params.receptiveField) });
		result.push_back(NameValue{ "weightsScale", BinaryWeightsScaleToString(params.weightsScale) });
        return result;
    }

    template<typename ElementType>
    std::vector<NameValue> InspectConvolutionalLayerParameters(const ell::predictors::neural::ConvolutionalLayer<ElementType>* layer)
    {
        std::vector<NameValue> result;
        auto params = layer->GetConvolutionalParameters();
        result.push_back(NameValue{ "stride", std::to_string(params.stride) });
        result.push_back(NameValue{ "method", ConvolutionMethodToString(params.method) });
		result.push_back(NameValue{ "receptiveField", std::to_string(params.receptiveField) });
		result.push_back(NameValue{ "numFilters", std::to_string(params.numFiltersAtATime) });
        return result;
    }

    template<typename ElementType, template <typename> class PoolingFunctionType>
    std::vector<NameValue> InspectPoolingLayerParameters(const ell::predictors::neural::PoolingLayer<ElementType, PoolingFunctionType>* layer)
    {
        std::vector<NameValue> result;
        auto params = layer->GetPoolingParameters();
        result.push_back(NameValue{ "stride", std::to_string(params.stride) });
        result.push_back(NameValue{ "size", std::to_string(params.poolingSize) });
        return result;
    }

    template<typename ElementType>
    std::vector<NameValue> InspectLayerParameters(std::shared_ptr < ell::predictors::neural::Layer<ElementType>> layer)
    {
        std::vector<NameValue> result;
        auto params = layer->GetLayerParameters();
        auto input = params.input;
        auto shape = params.outputShape;

		result.push_back(NameValue{ "shape",  "[" + std::to_string(input.NumRows()) + "," + std::to_string(input.NumColumns()) + "," + std::to_string(input.NumChannels()) + "]->" +
											  "[" + std::to_string(shape.NumRows()) + "," + std::to_string(shape.NumColumns()) + "," + std::to_string(shape.NumChannels()) + "]" });

        auto inputpadding = params.inputPaddingParameters;
        auto outputpadding = params.outputPaddingParameters;
        if (inputpadding.paddingSize != 0) {
            result.push_back(NameValue{ "inputPadding", PaddingSchemeToString(inputpadding.paddingScheme) + "," + std::to_string(inputpadding.paddingSize) });
        }
        if (outputpadding.paddingSize != 0) {
            result.push_back(NameValue{ "outputPadding", PaddingSchemeToString(outputpadding.paddingScheme) + "," + std::to_string(outputpadding.paddingSize) });
        }

        const ell::predictors::neural::BinaryConvolutionalLayer<ElementType>* bcl = dynamic_cast<const ell::predictors::neural::BinaryConvolutionalLayer<ElementType>*>(layer.get());
        if (bcl != nullptr)
        {
            std::vector<NameValue> more = InspectBinaryConvolutionalLayerParameters<ElementType>(bcl);
            result.insert(result.end(), more.begin(), more.end());
        }

        const ell::predictors::neural::ConvolutionalLayer<ElementType>* conv = dynamic_cast<const ell::predictors::neural::ConvolutionalLayer<ElementType>*>(layer.get());
        if (conv != nullptr)
        {
            std::vector<NameValue> more = InspectConvolutionalLayerParameters<ElementType>(conv);
            result.insert(result.end(), more.begin(), more.end());
        }

        const ell::predictors::neural::PoolingLayer<ElementType, ell::predictors::neural::MaxPoolingFunction>* maxpooling = dynamic_cast<const ell::predictors::neural::PoolingLayer<ElementType, ell::predictors::neural::MaxPoolingFunction>*>(layer.get());
        if (maxpooling != nullptr)
        {
            result.push_back(NameValue{ "function", "maxpooling" });
            std::vector<NameValue> more = InspectPoolingLayerParameters<ElementType, ell::predictors::neural::MaxPoolingFunction>(maxpooling);
            result.insert(result.end(), more.begin(), more.end());
        }

        const ell::predictors::neural::PoolingLayer<ElementType, ell::predictors::neural::MeanPoolingFunction>* meanpooling = dynamic_cast<const ell::predictors::neural::PoolingLayer<ElementType, ell::predictors::neural::MeanPoolingFunction>*>(layer.get());
        if (meanpooling != nullptr)
        {
            result.push_back(NameValue{ "function", "meanpooling" });
            std::vector<NameValue> more = InspectPoolingLayerParameters<ElementType, ell::predictors::neural::MeanPoolingFunction>(meanpooling);
            result.insert(result.end(), more.begin(), more.end());
        }

		return result;
    }

}
