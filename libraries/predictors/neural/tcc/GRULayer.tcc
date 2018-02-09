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
            : _updateWeights(0, 0), _resetWeights(0, 0), _hiddenWeights(0, 0), _updateBias(0), _resetBias(0), _hiddenBias(0), _inputPlusHidden(0)
        {
        }

        template <typename ElementType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
        GRULayer<ElementType, ActivationFunctionType, RecurrentActivationFunctionType>::GRULayer(const LayerParameters& layerParameters, GRUParameters<ElementType>& parameters)
            : Layer<ElementType>(layerParameters), _updateWeights(parameters.updateWeights), _resetWeights(parameters.resetWeights), _hiddenWeights(parameters.hiddenWeights), _updateBias(parameters.updateBias), _resetBias(parameters.resetBias), _hiddenBias(parameters.hiddenBias), _inputPlusHidden(layerParameters.input.Size() + GetOutputMinusPadding().Size())
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
                using namespace std::string_literals;
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Dimensionality of the biases must conform to the output shape of the network (bias: "s + std::to_string(_updateBias.Size()) + ", output: " + std::to_string(outputSize) + ")");
            }
        }

        // Notation:
        // The notation in the comments is adapted from the explanation at http://colah.github.io/posts/2015-08-Understanding-LSTMs/
        //
        // Wu == updateWeights
        // Wr == resetWeights
        // Wh == hiddenWeights (aka output weights)
        // Bu == updateBias
        // Br == resetBias
        // Bh == hiddenBias
        //
        // Zt == updateGateActivation
        // Rt == resetGateActivation
        // 
        // [Xt, Ht-1] == inputPlusHidden
        // Ht~ == newHiddenState
        // Ht == hiddenState (aka, output)
        //
        template <typename ElementType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
        void GRULayer<ElementType, ActivationFunctionType, RecurrentActivationFunctionType>::Compute()
        {
            auto& input = _layerParameters.input;
            auto output = GetOutputMinusPadding();

            size_t inputSize = input.Size();
            size_t outputSize = output.Size();

            VectorType updateGateActivation(outputSize);
            VectorType resetGateActivation(outputSize);
            VectorType newHiddenState(outputSize);
            VectorType prevHiddenState(outputSize);

            // inputPlusHidden = [Xt-1, Ht-1] here
            auto inputPart = _inputPlusHidden.GetSubVector(0, inputSize);
            auto hiddenPart = _inputPlusHidden.GetSubVector(inputSize, outputSize);

            // Reshape the input (Xt) and copy into inputPart
            size_t index = 0;
            for (size_t i = 0; i < input.NumRows(); ++i)
            {
                for (size_t j = 0; j < input.NumColumns(); ++j)
                {
                    for (size_t k = 0; k < input.NumChannels(); ++k)
                    {
                        inputPart[index++] = input(i, j, k);
                    }
                }
            }
            // Now, inputPlusHidden = [Xt, Ht-1]

            // Zt = recurrentFunction(Wu * [Xt, Ht-1] + Bu)   (where recurrentFunction is usually sigmoid)
            math::MultiplyScaleAddUpdate(static_cast<ElementType>(1), _updateWeights, _inputPlusHidden, static_cast<ElementType>(0), updateGateActivation);
            math::AddUpdate(_updateBias, updateGateActivation);
            _recurrentActivationFunction.Apply(updateGateActivation);

            // Rt = recurrentFunction(Wr * [Xt, Ht-1] + Br)   (where recurrentFunction is usually sigmoid)
            math::MultiplyScaleAddUpdate(static_cast<ElementType>(1), _resetWeights, _inputPlusHidden, static_cast<ElementType>(0), resetGateActivation);
            math::AddUpdate(_resetBias, resetGateActivation);
            _recurrentActivationFunction.Apply(resetGateActivation);

            // Ht~ = activationFunction(Wh * [Xt, (Rt .* Ht-1)] + Bh)   (where activationFunction is typically tanh)
            prevHiddenState.CopyFrom(hiddenPart); // make a copy of Ht-1
            math::ElementwiseMultiplySet(resetGateActivation, prevHiddenState, hiddenPart); // hiddenPart aliases to the last part of _inputPlusHidden
            math::MultiplyScaleAddUpdate(static_cast<ElementType>(1), _hiddenWeights, _inputPlusHidden, static_cast<ElementType>(0), newHiddenState);
            math::AddUpdate(_hiddenBias, newHiddenState);
            _activationFunction.Apply(newHiddenState);

            // Compute Ht = (1-Zt) .* Ht~ + Zt * Ht-1,
            index = 0;
            for (size_t i = 0; i < output.NumRows(); ++i)
            {
                for (size_t j = 0; j < output.NumColumns(); ++j)
                {
                    for (size_t k = 0; k < output.NumChannels(); ++k)
                    {
                        auto newValue = ((1 - updateGateActivation[index]) * newHiddenState[index]) + (updateGateActivation[index] * prevHiddenState[index]);
                        hiddenPart[index] = newValue;
                        output(i, j, k) = newValue;
                        ++index;
                    }
                }
            }
        }

        template <typename ElementType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
        void GRULayer<ElementType, ActivationFunctionType, RecurrentActivationFunctionType>::Reset()
        {
            const auto outputSize = GetOutputMinusPadding().Size();
            const auto inputSize = _layerParameters.input.Size();
            auto ht = _inputPlusHidden.GetSubVector(inputSize, outputSize);
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

            _inputPlusHidden.Resize(_layerParameters.input.Size() + _updateBias.Size());
        }
    }
}
}
