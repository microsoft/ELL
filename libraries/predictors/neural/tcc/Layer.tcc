////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Layer.tcc (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Layer.h"

// stl
#include <ostream>
#include <limits>
#include <type_traits>

namespace ell
{
namespace predictors
{
namespace neural
{
    template <typename ValueType>
    ValueType GetPaddingValue(PaddingScheme paddingScheme)
    {
        switch (paddingScheme)
        {
            case PaddingScheme::zeros:
                return static_cast<ValueType>(0);
            case PaddingScheme::minusOnes:
                return static_cast<ValueType>(-1);
            case PaddingScheme::alternatingZeroAndOnes:
                return static_cast<ValueType>(0);
            case PaddingScheme::randomZeroAndOnes:
                return static_cast<ValueType>(0);
            case PaddingScheme::min:
                return -std::numeric_limits<ValueType>::max();
            case PaddingScheme::max:
                return std::numeric_limits<ValueType>::max();
        }
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Invalid PaddingScheme");
    }

    //
    // Layer
    //
    template <typename ElementType>
    Layer<ElementType>::Layer(const LayerParameters& layerParameters) :
        _layerParameters(layerParameters),
        _output(layerParameters.outputShape)
    {
        InitializeOutputValues(_output, layerParameters.outputPaddingParameters);
    }

    template <typename ElementType>
    typename Layer<ElementType>::Shape Layer<ElementType>::GetInputShapeMinusPadding() const
    {
        auto&& inputShape = _layerParameters.input.GetShape(); 
        auto paddingSize = _layerParameters.inputPaddingParameters.paddingSize;
        if(inputShape[0] < 2*paddingSize || inputShape[1] < 2*paddingSize)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "Input size not large enough to accomodate padding");
        }       
        return { inputShape[0]-2*paddingSize, inputShape[1]-2*paddingSize, inputShape[2] };
    }

    template <typename ElementType>
    typename Layer<ElementType>::Shape Layer<ElementType>::GetOutputShapeMinusPadding() const
    {
        auto&& outputShape = _layerParameters.outputShape; 
        auto paddingSize = _layerParameters.outputPaddingParameters.paddingSize;
        if(outputShape[0] < 2*paddingSize || outputShape[1] < 2*paddingSize)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "Output size not large enough to accomodate padding");
        }
        return { outputShape[0]-2*paddingSize, outputShape[1]-2*paddingSize, outputShape[2] };
    }

    template <typename ElementType>
    void Layer<ElementType>::InitializeOutputValues(TensorType& output, PaddingParameters outputPaddingParameters)
    {
        switch (outputPaddingParameters.paddingScheme)
        {
            case PaddingScheme::minusOnes:
                output.Fill(-1);
                break;
            case PaddingScheme::randomZeroAndOnes:
                output.Generate([] { return static_cast<ElementType>(std::rand() % 2); });
                break;
            case PaddingScheme::alternatingZeroAndOnes:
                {
                    for (size_t row = 0; row < output.NumRows(); row++)
                    {
                        for (size_t column = 0; column < output.NumColumns(); column++)
                        {
                            ElementType value = static_cast<ElementType>((row % 2) ^ (column % 2));
                            for (size_t channel = 0; channel < output.NumChannels(); channel++)
                            {
                                output(row, column, channel) = value;
                            }
                        }
                    }
                }
                break;
            case PaddingScheme::min:
                if (std::is_signed<ElementType>::value)
                {
                    output.Fill(-std::numeric_limits<ElementType>::max());
                }
                else
                {
                    output.Fill(std::numeric_limits<ElementType>::min());
                }
                break;
            case PaddingScheme::max:
                output.Fill(std::numeric_limits<ElementType>::max());
                break;
            default:
                output.Fill(0);
                break;
        }
    }

    template <typename ElementType>
    void Layer<ElementType>::Print(std::ostream& os, size_t numValuesToPrint) const
    {
        static constexpr size_t bufferLength = 1024;
        char buffer[bufferLength] = {0};
        std::string layerName = LayerNames[static_cast<uint32_t>(GetLayerType())];

        snprintf(buffer, bufferLength, "======== %s layer (%zd x %zd x %zd) pad: %zd -> (%zd x %zd x %zd) pad: %zd ========",
            layerName.c_str(),
            _layerParameters.input.NumRows() - 2 * _layerParameters.inputPaddingParameters.paddingSize, _layerParameters.input.NumColumns() - 2 * _layerParameters.inputPaddingParameters.paddingSize, _layerParameters.input.NumChannels(), _layerParameters.inputPaddingParameters.paddingSize,
            _layerParameters.outputShape[0] - 2 * _layerParameters.outputPaddingParameters.paddingSize, _layerParameters.outputShape[1] - 2 * _layerParameters.outputPaddingParameters.paddingSize, _layerParameters.outputShape[2], _layerParameters.outputPaddingParameters.paddingSize);

        os << buffer;

        const ConstTensorReferenceType output(_output);
        for (size_t i = 0; (i < numValuesToPrint) && (i < output.Size()); i++)
        {
            size_t channel = i % output.NumChannels();
            size_t col = i / output.NumChannels();
            size_t row = i / (output.NumChannels() * output.NumColumns());

            if (i % 10 == 0) os << std::endl;

            if (channel < output.NumChannels() &&
                (col + _layerParameters.outputPaddingParameters.paddingSize) < output.NumColumns() &&
                (row + _layerParameters.outputPaddingParameters.paddingSize) < output.NumRows())
            {
                const ElementType val = output({ row + _layerParameters.outputPaddingParameters.paddingSize, col + _layerParameters.outputPaddingParameters.paddingSize, channel });
                snprintf(buffer, bufferLength, "%+9.5f ", val);
                os << buffer;
            }
        }
        os << std::endl << "======== End of " << layerName << " ========" << std::endl;
    }

    template <typename ElementType>
    void Layer<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["inputPaddingScheme"] << static_cast<int>(_layerParameters.inputPaddingParameters.paddingScheme);
        archiver["inputPaddingSize"] << _layerParameters.inputPaddingParameters.paddingSize;

        archiver["outputShape"] << std::vector<size_t>(_layerParameters.outputShape.begin(), _layerParameters.outputShape.end());

        archiver["outputPaddingScheme"] << static_cast<int>(_layerParameters.outputPaddingParameters.paddingScheme);
        archiver["outputPaddingSize"] << _layerParameters.outputPaddingParameters.paddingSize;
    }

    template <typename ElementType>
    void Layer<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        int inputPaddingScheme;
        archiver["inputPaddingScheme"] >> inputPaddingScheme;
        _layerParameters.inputPaddingParameters.paddingScheme = static_cast<PaddingScheme>(inputPaddingScheme);
        archiver["inputPaddingSize"] >> _layerParameters.inputPaddingParameters.paddingSize;

        std::vector<size_t> outputShape;
        archiver["outputShape"] >> outputShape;
        std::copy(outputShape.begin(), outputShape.end(), _layerParameters.outputShape.begin());

        int outputPaddingScheme;
        archiver["outputPaddingScheme"] >> outputPaddingScheme;
        _layerParameters.outputPaddingParameters.paddingScheme = static_cast<PaddingScheme>(outputPaddingScheme);
        archiver["outputPaddingSize"] >> _layerParameters.outputPaddingParameters.paddingSize;

        _output = TensorType(_layerParameters.outputShape);

        LayerSerializationContext<ElementType>* layerContext = dynamic_cast<LayerSerializationContext<ElementType>*>(&archiver.GetContext());
        if(layerContext != nullptr)
        {
            // Set the input reference to the previously restored layer's output. This is saved in the
            // serialization context
            _layerParameters.input = layerContext->GetPreviousOutputReference();

            // Save the output reference to the serialization context
            layerContext->SetOutputReference(GetOutput());
        }
    }

    template <typename ElementType>
    typename Layer<ElementType>::TensorReferenceType Layer<ElementType>::GetOutputMinusPadding()
    { 
        return _output.GetSubTensor({ _layerParameters.outputPaddingParameters.paddingSize, _layerParameters.outputPaddingParameters.paddingSize, 0 }, { _output.NumRows() - 2 * _layerParameters.outputPaddingParameters.paddingSize, _output.NumColumns() - 2 * _layerParameters.outputPaddingParameters.paddingSize, _output.NumChannels() });
    }

    template <typename ElementType>
    void Layer<ElementType>::AssignValues(ConstTensorReferenceType& input, TensorReferenceType& output)
    {
        DEBUG_THROW(input.NumRows() > output.NumRows() || input.NumColumns() > output.NumColumns() || input.NumChannels() > output.NumChannels(), utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "Input tensor must not exceed output tensor dimensions."));

        for (size_t i = 0; i < input.NumRows(); i++)
        {
            for (size_t j = 0; j < input.NumColumns(); j++)
            {
                for (size_t k = 0; k < input.NumChannels(); k++)
                {
                    output(i, j, k) = input(i, j, k);
                }
            }
        }
    }
}
}
}

