////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RecurrentLayer.tcc (neural)
//  Authors:  James Devine
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace predictors
{
    namespace neural
    {
        template <typename ElementType, template <typename> class ActivationFunctionType>
        RecurrentLayer<ElementType, ActivationFunctionType>::RecurrentLayer()
            : _hiddenWeights(0, 0), _hiddenBias(0), _inputPlusHiddenVector(0)
        {
        }

        template <typename ElementType, template <typename> class ActivationFunctionType>
        RecurrentLayer<ElementType, ActivationFunctionType>::RecurrentLayer(const LayerParameters& layerParameters, MatrixType& weights, VectorType& biases)
            : Layer<ElementType>(layerParameters), _hiddenWeights(weights), _hiddenBias(biases), _inputPlusHiddenVector(weights.NumColumns())
        {
            // verify parameters
            if (_hiddenWeights.NumRows() != _hiddenBias.Size())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Dimensionality of weights must be the same as the: [output dimension x (output dimension + input dimension)]");
            }
        }

        template <typename ElementType, template <typename> class ActivationFunctionType>
        void RecurrentLayer<ElementType, ActivationFunctionType>::Compute()
        {
            auto& input = _layerParameters.input;
            const auto inputSize = input.Size();
            auto output = GetOutputMinusPadding();
            const auto outputSize = output.Size();

            // Get references to the inputVector and Ht parts of _inputPlusHiddenVector
            auto inputVector = _inputPlusHiddenVector.GetSubVector(0, inputSize);

            // Reshape the input into a vector
            size_t columnIndex = 0;
            for (size_t i = 0; i < input.NumRows(); i++)
            {
                for (size_t j = 0; j < input.NumColumns(); j++)
                {
                    for (size_t k = 0; k < input.NumChannels(); k++)
                    {
                        inputVector[columnIndex++] = input(i, j, k);
                    }
                }
            }
            
            // Ht = activationFunction(Wh * [Xt, Ht-1] + Bh)
            
            // Matrix Multiply
            VectorType htNew(outputSize);
            math::MultiplyScaleAddUpdate(static_cast<ElementType>(1), _hiddenWeights, _inputPlusHiddenVector, static_cast<ElementType>(0), htNew);
            math::AddUpdate(_hiddenBias, htNew);
            _activation.Apply(htNew);

            // Update ht part of _inputPlusHiddenVector, and copy to reshaped output
            auto htOld = _inputPlusHiddenVector.GetSubVector(inputSize, outputSize);
            columnIndex = 0;
            for (size_t i = 0; i < output.NumRows(); i++)
            {
                for (size_t j = 0; j < output.NumColumns(); j++)
                {
                    for (size_t k = 0; k < output.NumChannels(); k++)
                    {
                        auto htVal = htNew[columnIndex];
                        output(i, j, k) = htVal;
                        htOld[columnIndex] = htVal;
                        ++columnIndex;
                    }
                }
            }
        }

        template <typename ElementType, template <typename> class ActivationFunctionType>
        void RecurrentLayer<ElementType, ActivationFunctionType>::Reset()
        {
            const auto outputSize = GetOutputMinusPadding().Size();
            const auto inputSize = _layerParameters.input.Size();
            auto ht = _inputPlusHiddenVector.GetSubVector(inputSize, outputSize);
            ht.Reset();
        }

        template <typename ElementType, template <typename> class ActivationFunctionType>
        void RecurrentLayer<ElementType, ActivationFunctionType>::WriteToArchive(utilities::Archiver& archiver) const
        {
            Layer<ElementType>::WriteToArchive(archiver);

            math::MatrixArchiver::Write(_hiddenWeights, "hiddenWeights", archiver);
            math::VectorArchiver::Write(_hiddenBias, "hiddenBias", archiver);
            _activation.WriteToArchive(archiver);
        }

        template <typename ElementType, template <typename> class ActivationFunctionType>
        void RecurrentLayer<ElementType, ActivationFunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
        {
            Layer<ElementType>::ReadFromArchive(archiver);

            math::MatrixArchiver::Read(_hiddenWeights, "hiddenWeights", archiver);
            math::VectorArchiver::Read(_hiddenBias, "hiddenBias", archiver);
            _activation.ReadFromArchive(archiver);
            _inputPlusHiddenVector.Resize(_hiddenWeights.NumColumns());
        }
    }
}
}
