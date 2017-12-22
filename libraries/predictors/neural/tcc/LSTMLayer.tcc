////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LSTMLayer.tcc (neural)
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
        LSTMLayer<ElementType, ActivationFunctionType, RecurrentActivationFunctionType>::LSTMLayer()
            : _inputWeights(0, 0), _forgetMeWeights(0, 0), _candidateWeights(0, 0), _outputWeights(0, 0), _inputBias(0), _forgetMeBias(0), _candidateBias(0), _outputBias(0), _inputPlusHiddenVector(0), _ctActual(0)
        {
        }

        template <typename ElementType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
        LSTMLayer<ElementType, ActivationFunctionType, RecurrentActivationFunctionType>::LSTMLayer(const LayerParameters& layerParameters, LSTMParameters<ElementType>& parameters)
            : Layer<ElementType>(layerParameters), _inputWeights(parameters.inputWeights), _forgetMeWeights(parameters.forgetMeWeights), _candidateWeights(parameters.candidateWeights), _outputWeights(parameters.outputWeights), _inputBias(parameters.inputBias), _forgetMeBias(parameters.forgetMeBias), _candidateBias(parameters.candidateBias), _outputBias(parameters.outputBias), _inputPlusHiddenVector(layerParameters.input.Size() + _inputBias.Size()), _ctActual(GetOutputMinusPadding().Size())
        {
            // verify parameters
            if (_inputWeights.NumColumns() != _forgetMeWeights.NumColumns() || _inputWeights.NumColumns() != _candidateWeights.NumColumns() || _inputWeights.NumColumns() != _outputWeights.NumColumns())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Dimensionality of all weights must be the same, weight matrices should be formatted as [Weights, Recurrent Weights] or [W, U]");
            }

            if (_inputWeights.NumRows() != _forgetMeWeights.NumRows() || _inputWeights.NumRows() != _candidateWeights.NumRows() || _inputWeights.NumRows() != _outputWeights.NumRows())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Dimensionality of all weights must be the same, weight matrices should be formatted as [Weights, Recurrent Weights] or [W, U]");
            }

            const auto outputSize = GetOutputMinusPadding().Size();
            if (_inputBias.Size() != outputSize || _forgetMeBias.Size() != outputSize || _candidateBias.Size() != outputSize || _outputBias.Size() != outputSize)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Dimensionality of the biases must conform to the output shape of the network.");
            }
        }

        template <typename ElementType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
        void LSTMLayer<ElementType, ActivationFunctionType, RecurrentActivationFunctionType>::Compute()
        {
            auto& input = _layerParameters.input;
            const auto inputSize = input.Size();
            auto output = GetOutputMinusPadding();
            const auto outputSize = output.Size();

            VectorType ft(outputSize);
            VectorType it(outputSize);
            VectorType ctNew(outputSize);
            VectorType ot(outputSize);

            auto inputPart = _inputPlusHiddenVector.GetSubVector(0, inputSize);
            auto htPart = _inputPlusHiddenVector.GetSubVector(inputSize, outputSize);

            // Reshape the input into a single vector
            size_t columnIndex = 0;
            for (size_t i = 0; i < input.NumRows(); i++)
            {
                for (size_t j = 0; j < input.NumColumns(); j++)
                {
                    for (size_t k = 0; k < input.NumChannels(); k++)
                    {
                        inputPart[columnIndex++] = input(i, j, k);
                    }
                }
            }

            // concatenate hidden and input values into [Xt, Ht-1]
            // htPart is Ht-1 at this point
            //    ___
            //    |i|
            //    |i|
            //    |i|
            //    |i|
            //    |h|
            //    |h|
            //    |h|
            //    ---

            // forget me value
            // ft = recurrentFunction(Wf * [Xt, Ht-1] + Bf)    (where recurrentFunction is usually sigmoid)
            //                    ___
            //                    |i|
            // _______________    |i|     ____
            // |w|w|w|w|w|w|w|    |i|     |ft|
            // |w|w|w|w|w|w|w| x  |i|   = |ft|
            // |w|w|w|w|w|w|w|    |h|     |ft|
            // ---------------    |h|     ----
            //                    |h|
            //                    ---
            math::MultiplyScaleAddUpdate(static_cast<ElementType>(1), _forgetMeWeights, _inputPlusHiddenVector, static_cast<ElementType>(0), ft);
            math::AddUpdate(_forgetMeBias, ft);
            _recurrentActivationFunction.Apply(ft);

            // update inputs
            // it = recurrentFunction(Wi * [Xt, Ht-1] + Bi)    (where recurrentFunction is usually sigmoid)
            //                    ___
            //                    |i|
            // _______________    |i|     ____
            // |w|w|w|w|w|w|w|    |i|     |it|
            // |w|w|w|w|w|w|w| x  |i|   = |it|
            // |w|w|w|w|w|w|w|    |h|     |it|
            // ---------------    |h|     ----
            //                    |h|
            //                    ---
            math::MultiplyScaleAddUpdate(static_cast<ElementType>(1), _inputWeights, _inputPlusHiddenVector, static_cast<ElementType>(0), it);
            math::AddUpdate(_inputBias, it);
            _recurrentActivationFunction.Apply(it);

            // create new candidate values
            // Ct~ = activationFunction(Wc * [Xt, Ht-1] + Bc)  (where activationFunction is usually tanh)
            //                    ___
            //                    |i|
            // _______________    |i|     _____
            // |w|w|w|w|w|w|w|    |i|     |ct~|
            // |w|w|w|w|w|w|w| x  |i|   = |ct~|
            // |w|w|w|w|w|w|w|    |h|     |ct~|
            // ---------------    |h|     -----
            //                    |h|
            //                    ---
            math::MultiplyScaleAddUpdate(static_cast<ElementType>(1), _candidateWeights, _inputPlusHiddenVector, static_cast<ElementType>(0), ctNew);
            math::AddUpdate(_candidateBias, ctNew);
            _activationFunction.Apply(ctNew);

            // calculate new cell state using previous values (ctActual)
            // Ct = ft * Ct-1 + it * Ct~
            for (size_t i = 0; i < _ctActual.Size(); i++)
            {
                _ctActual[i] = ft[i] * _ctActual[i] + it[i] * ctNew[i];
            }

            // update the output values
            // ot = recurrentFunction(Wo [Xt, Ht-1] + Bo)
            //                    ___
            //                    |i|
            // _______________    |i|     ____
            // |w|w|w|w|w|w|w|    |i|     |ot|
            // |w|w|w|w|w|w|w| x  |i|   = |ot|
            // |w|w|w|w|w|w|w|    |h|     |ot|
            // ---------------    |h|     ----
            //                    |h|
            //                    ---
            math::MultiplyScaleAddUpdate(static_cast<ElementType>(1), _outputWeights, _inputPlusHiddenVector, static_cast<ElementType>(0), ot);
            math::AddUpdate(_outputBias, ot);
            _recurrentActivationFunction.Apply(ot);

            // compute the hidden layer and copy into state vector
            // ht = ot * activation(Ct)
            // __________
            // |ht|ht|ht|
            // ----------
            for (size_t i = 0; i < _ctActual.Size(); i++)
            {
                htPart[i] = ot[i] * _activationFunction.Apply(_ctActual[i]);
            }

            // Copy ht into reshaped output
            columnIndex = 0;
            for (size_t i = 0; i < output.NumRows(); i++)
            {
                for (size_t j = 0; j < output.NumColumns(); j++)
                {
                    for (size_t k = 0; k < output.NumChannels(); k++)
                    {
                        output(i, j, k) = htPart[columnIndex];
                        ++columnIndex;
                    }
                }
            }
        }

        template <typename ElementType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
        void LSTMLayer<ElementType, ActivationFunctionType, RecurrentActivationFunctionType>::Reset()
        {
            const auto outputSize = GetOutputMinusPadding().Size();
            const auto inputSize = _layerParameters.input.Size();
            auto ht = _inputPlusHiddenVector.GetSubVector(inputSize, outputSize);
            ht.Reset();
            _ctActual.Reset();
        }

        template <typename ElementType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
        void LSTMLayer<ElementType, ActivationFunctionType, RecurrentActivationFunctionType>::WriteToArchive(utilities::Archiver& archiver) const
        {
            Layer<ElementType>::WriteToArchive(archiver);

            math::MatrixArchiver::Write(_inputWeights, "inputWeights", archiver);
            math::MatrixArchiver::Write(_forgetMeWeights, "forgottenWeights", archiver);
            math::MatrixArchiver::Write(_candidateWeights, "candidateWeights", archiver);
            math::MatrixArchiver::Write(_outputWeights, "outputWeights", archiver);

            math::VectorArchiver::Write(_inputBias, "inputBias", archiver);
            math::VectorArchiver::Write(_forgetMeBias, "forgottenBias", archiver);
            math::VectorArchiver::Write(_candidateBias, "candidateBias", archiver);
            math::VectorArchiver::Write(_outputBias, "outputBias", archiver);

            _activationFunction.WriteToArchive(archiver);
            _recurrentActivationFunction.WriteToArchive(archiver);
        }

        template <typename ElementType, template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
        void LSTMLayer<ElementType, ActivationFunctionType, RecurrentActivationFunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
        {
            Layer<ElementType>::ReadFromArchive(archiver);

            math::MatrixArchiver::Read(_inputWeights, "inputWeights", archiver);
            math::MatrixArchiver::Read(_forgetMeWeights, "forgottenWeights", archiver);
            math::MatrixArchiver::Read(_candidateWeights, "candidateWeights", archiver);
            math::MatrixArchiver::Read(_outputWeights, "outputWeights", archiver);

            math::VectorArchiver::Read(_inputBias, "inputBias", archiver);
            math::VectorArchiver::Read(_forgetMeBias, "forgottenBias", archiver);
            math::VectorArchiver::Read(_candidateBias, "candidateBias", archiver);
            math::VectorArchiver::Read(_outputBias, "outputBias", archiver);

            _activationFunction.ReadFromArchive(archiver);
            _recurrentActivationFunction.ReadFromArchive(archiver);

            _inputPlusHiddenVector.Resize(_layerParameters.input.Size() + _inputBias.Size());
        }
    }
}
}
