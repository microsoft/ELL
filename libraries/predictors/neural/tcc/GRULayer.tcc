////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GRULayer.tcc (neural)
//  Authors:  James Devine
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace predictors
{
    namespace neural
    {
        template <typename ElementType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
        GRULayer<ElementType, ActivationFunctionType, RecurrentActivationFunctionType>::GRULayer()
            : _updateWeights(0, 0), _resetWeights(0, 0), _hiddenWeights(0, 0), _updateBias(0), _resetBias(0), _hiddenBias(0), _inputPlusHiddenVector(0)
        {
        }

        template <typename ElementType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
        GRULayer<ElementType, ActivationFunctionType, RecurrentActivationFunctionType>::GRULayer(const LayerParameters& layerParameters, GRUParameters<ElementType>& parameters)
            : Layer<ElementType>(layerParameters), _updateWeights(parameters.updateWeights), _resetWeights(parameters.resetWeights), _hiddenWeights(parameters.hiddenWeights), _updateBias(parameters.updateBias), _resetBias(parameters.resetBias), _hiddenBias(parameters.hiddenBias), _inputPlusHiddenVector(layerParameters.input.Size() + GetOutputMinusPadding().Size())
        {
            const auto outputSize = GetOutputMinusPadding().Size();

            // verify parameters
            if (_updateWeights.NumColumns() != _resetWeights.NumColumns() || _updateWeights.NumColumns() != _hiddenWeights.NumColumns())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Dimensionality of all weights must be the same, weight matrices should be formatted as [Weights, Recurrent Weights] or [W, U]");
            }

            if (_updateWeights.NumRows() != _resetWeights.NumRows() || _updateWeights.NumRows() != _hiddenWeights.NumRows())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Dimensionality of all weights must be the same, weight matrices should be formatted as [Weights, Recurrent Weights] or [W, U]");
            }

            if (_hiddenWeights.NumRows() != _hiddenBias.Size()) // || _hiddenWeights.NumColumns() != _inputVector.Size() + outputSize)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Dimensionality of weights must be the same as the: [output dimension x (output dimension + input dimension)]");
            }

            if (_updateBias.Size() != outputSize || _resetBias.Size() != outputSize || _hiddenBias.Size() != outputSize)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Dimensionality of the biases must conform to the output shape of the network.");
            }
        }

        template <typename ElementType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
        void GRULayer<ElementType, ActivationFunctionType, RecurrentActivationFunctionType>::Compute()
        {
            auto output = GetOutputMinusPadding();
            auto& input = _layerParameters.input;

            size_t outputSize = output.Size();
            size_t inputSize = input.Size();

            VectorType zt(outputSize);
            VectorType rt(outputSize);
            VectorType htNew(outputSize);
            VectorType htOld(outputSize);

            // inputPlusHiddenVector = [Xt-1, Ht−1] here
            auto inputPart = _inputPlusHiddenVector.GetSubVector(0, inputSize);
            auto htPart = _inputPlusHiddenVector.GetSubVector(inputSize, outputSize);

            // Reshape the input (Xt) and copy into inputPart
            size_t columnIndex = 0;
            for (size_t i = 0; i < input.NumRows(); ++i)
            {
                for (size_t j = 0; j < input.NumColumns(); ++j)
                {
                    for (size_t k = 0; k < input.NumChannels(); ++k)
                    {
                        inputPart[columnIndex++] = input(i, j, k);
                    }
                }
            }
            // Now, inputPlusHiddenVector = [Xt, Ht−1]

            // Zt = recurrentFunction(Wz * [Xt, Ht−1] + Bz)   (where recurrentFunction is usually sigmoid)
            math::MultiplyScaleAddUpdate(static_cast<ElementType>(1), _updateWeights, _inputPlusHiddenVector, static_cast<ElementType>(0), zt);
            math::AddUpdate(_updateBias, zt);
            _recurrentActivationFunction.Apply(zt);

            // Rt = recurrentFunction(Wr * [Xt, Ht-1] + Br)   (where recurrentFunction is usually sigmoid)
            math::MultiplyScaleAddUpdate(static_cast<ElementType>(1), _resetWeights, _inputPlusHiddenVector, static_cast<ElementType>(0), rt);
            math::AddUpdate(_resetBias, rt);
            _recurrentActivationFunction.Apply(rt);

            htOld.CopyFrom(htPart); // make a copy of Ht-1
            auto& rtHt = htPart; // reusing htPart with a new name, so the code follows the math in comments
            // Ht~ = activationFunction(Wh * [Xt, Rt * Ht-1] + Bh)   (where activationFunction is typically tanh)
            math::ElementwiseMultiplySet(rt, htOld, rtHt); // rtHt aliases to the last part of _inputPlusHiddenVector
            math::MultiplyScaleAddUpdate(static_cast<ElementType>(1), _hiddenWeights, _inputPlusHiddenVector, static_cast<ElementType>(0), htNew);
            math::AddUpdate(_hiddenBias, htNew);
            _activationFunction.Apply(htNew);

            // Reshape the output and set Ht = (1−Zt) Ht~ + Zt * Ht-1,
            columnIndex = 0;
            for (size_t i = 0; i < output.NumRows(); ++i)
            {
                for (size_t j = 0; j < output.NumColumns(); ++j)
                {
                    for (size_t k = 0; k < output.NumChannels(); ++k)
                    {
                        auto newValue = (1 - zt[columnIndex]) * htNew[columnIndex] + zt[columnIndex] * htOld[columnIndex];
                        htPart[columnIndex] = newValue;
                        output(i, j, k) = newValue;
                        ++columnIndex;
                    }
                }
            }
        }

        template <typename ElementType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
        void GRULayer<ElementType, ActivationFunctionType, RecurrentActivationFunctionType>::Reset()
        {
            const auto outputSize = GetOutputMinusPadding().Size();
            const auto inputSize = _layerParameters.input.Size();
            auto ht = _inputPlusHiddenVector.GetSubVector(inputSize, outputSize);
            ht.Reset();
        }

        template <typename ElementType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
        void GRULayer<ElementType, ActivationFunctionType, RecurrentActivationFunctionType>::WriteToArchive(utilities::Archiver& archiver) const
        {
            Layer<ElementType>::WriteToArchive(archiver);

            math::MatrixArchiver::Write(_updateWeights, "updateWeights", archiver);
            math::MatrixArchiver::Write(_resetWeights, "resetWeights", archiver);
            math::MatrixArchiver::Write(_hiddenWeights, "hiddenWeights", archiver);

            math::VectorArchiver::Write(_updateBias, "updateBias", archiver);
            math::VectorArchiver::Write(_resetBias, "resetBias", archiver);
            math::VectorArchiver::Write(_hiddenBias, "hiddenBias", archiver);

            _activationFunction.WriteToArchive(archiver);
            _recurrentActivationFunction.WriteToArchive(archiver);
        }

        template <typename ElementType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
        void GRULayer<ElementType, ActivationFunctionType, RecurrentActivationFunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
        {
            Layer<ElementType>::ReadFromArchive(archiver);

            math::MatrixArchiver::Read(_updateWeights, "updateWeights", archiver);
            math::MatrixArchiver::Read(_resetWeights, "resetWeights", archiver);
            math::MatrixArchiver::Read(_hiddenWeights, "hiddenWeights", archiver);

            math::VectorArchiver::Read(_updateBias, "updateBias", archiver);
            math::VectorArchiver::Read(_resetBias, "resetBias", archiver);
            math::VectorArchiver::Read(_hiddenBias, "hiddenBias", archiver);

            _activationFunction.ReadFromArchive(archiver);
            _recurrentActivationFunction.ReadFromArchive(archiver);

            _inputPlusHiddenVector.Resize(_layerParameters.input.Size() + _updateBias.Size());
        }
    }
}
}
