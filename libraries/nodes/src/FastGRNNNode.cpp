////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FastGRNNNode.cpp (nodes)
//  Authors:  Yash Gaurkar, Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FastGRNNNode.h"
#include "ActivationFunctions.h"

#include <emitters/include/IRMath.h>

#include <math/include/MatrixOperations.h>

#include <utilities/include/Exception.h>

#include <value/include/EmitterContext.h>
#include <value/include/LLVMContext.h>
#include <value/include/Matrix.h>
#include <value/include/MatrixOperations.h>

namespace ell
{
namespace nodes
{
    using namespace utilities;
    using namespace value;

    template <typename ElementType>
    FastGRNNNode<ElementType>::FastGRNNNode() :
        CompilableCodeNode("FastGRNNNode",
                           { &_input, &_resetTrigger, &_inputWeights1, &_inputWeights2, &_updateWeights1, &_updateWeights2, &_biasGate, &_biasUpdate, &_zeta, &_nu },
                           { &_output }),
        _input(this, {}, defaultInputPortName),
        _resetTrigger(this, resetTriggerPortName),
        _hiddenUnits(0),
        _wRank(0),
        _uRank(0),
        _inputWeights1(this, {}, W1PortName),
        _inputWeights2(this, {}, W2PortName),
        _updateWeights1(this, {}, U1PortName),
        _updateWeights2(this, {}, U2PortName),
        _biasGate(this, {}, biasGatePortName),
        _biasUpdate(this, {}, biasUpdatePortName),
        _zeta(this, {}, zetaPortName),
        _nu(this, {}, nuPortName),
        _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ElementType>
    FastGRNNNode<ElementType>::FastGRNNNode(const model::OutputPort<ElementType>& input,
                                            const model::OutputPortBase& resetTrigger,
                                            size_t hiddenUnits,
                                            size_t wRank,
                                            size_t uRank,
                                            const model::OutputPort<ElementType>& inputWeights1,
                                            const model::OutputPort<ElementType>& inputWeights2,
                                            const model::OutputPort<ElementType>& updateWeights1,
                                            const model::OutputPort<ElementType>& updateWeights2,
                                            const model::OutputPort<ElementType>& biasGate,
                                            const model::OutputPort<ElementType>& biasUpdate,
                                            const model::OutputPort<ElementType>& zeta,
                                            const model::OutputPort<ElementType>& nu,
                                            const ActivationType& gateActivation,
                                            const ActivationType& updateActivation) :
        CompilableCodeNode("FastGRNNNode", { &_input, &_resetTrigger, &_inputWeights1, &_inputWeights2, &_updateWeights1, &_updateWeights2, &_biasGate, &_biasUpdate, &_zeta, &_nu }, { &_output }),
        _input(this, input, defaultInputPortName),
        _resetTrigger(this, resetTrigger, resetTriggerPortName),
        _hiddenUnits(hiddenUnits),
        _wRank(wRank),
        _uRank(uRank),
        _inputWeights1(this, inputWeights1, W1PortName),
        _inputWeights2(this, inputWeights2, W2PortName),
        _updateWeights1(this, updateWeights1, U1PortName),
        _updateWeights2(this, updateWeights2, U2PortName),
        _biasGate(this, biasGate, biasGatePortName),
        _biasUpdate(this, biasUpdate, biasUpdatePortName),
        _zeta(this, zeta, zetaPortName),
        _nu(this, nu, nuPortName),
        _output(this, defaultOutputPortName, hiddenUnits),
        _gateActivation(gateActivation),
        _updateActivation(updateActivation)
    {
        ValidateWeights();
    }

    template <typename ElementType>
    void FastGRNNNode<ElementType>::ValidateWeights() const
    {
        size_t numRows = _hiddenUnits;
        size_t wrank = _wRank;
        size_t urank = _uRank;
        size_t numColumns = input.Size();
        if (wrank == 0)
        {
            if (_inputWeights1.Size() != numRows * numColumns)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                                ell::utilities::FormatString("The FastGRNNNode input weights are the wrong size, found %zu but expecting %zu", _inputWeights1.Size(), numRows * numColumns));
            }
        }
        else
        {
            if (_inputWeights1.Size() != wrank * numColumns)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                                ell::utilities::FormatString("The FastGRNNNode input weights1 are the wrong size, found %zu but expecting %zu", _inputWeights1.Size(), wrank * numColumns));
            }
            if (_inputWeights2.Size() != numRows * wrank)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                                ell::utilities::FormatString("The FastGRNNNode input weights2 are the wrong size, found %zu but expecting %zu", _inputWeights2.Size(), numRows * wrank));
            }
        }
        numColumns = _hiddenUnits;
        if (urank == 0)
        {
            if (_updateWeights1.Size() != numRows * numColumns)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                                ell::utilities::FormatString("The FastGRNNNode update weights are the wrong size, found %zu but expecting %zu", _updateWeights1.Size(), numRows * numColumns));
            }
        }
        else
        {
            if (_updateWeights1.Size() != urank * numColumns)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                                ell::utilities::FormatString("The FastGRNNNode update weights1 are the wrong size, found %zu but expecting %zu", _updateWeights1.Size(), urank * numColumns));
            }
            if (_updateWeights2.Size() != numRows * urank)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                                ell::utilities::FormatString("The FastGRNNNode update weights2 are the wrong size, found %zu but expecting %zu", _updateWeights2.Size(), numRows * urank));
            }
        }
        if (_biasGate.Size() != _hiddenUnits)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                            ell::utilities::FormatString("The FastGRNNNode input biasGate vector is the wrong size, found %zu but expecting %zu", _biasGate.Size(), _hiddenUnits));
        }

        if (_biasUpdate.Size() != _hiddenUnits)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                            ell::utilities::FormatString("The FastGRNNNode hidden biasUpdate vector is the wrong size, found %zu but expecting %zu", _biasUpdate.Size(), _hiddenUnits));
        }
        if (_zeta.Size() != 1)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                            ell::utilities::FormatString("The FastGRNNNode zeta is the wrong size, found %zu but expecting %zu", zeta.Size(), 1));
        }
        if (_nu.Size() != 1)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                            ell::utilities::FormatString("The FastGRNNNode nu is the wrong size, found %zu but expecting %zu", nu.Size(), 1));
        }
    }

    template <typename ElementType>
    void FastGRNNNode<ElementType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInput = transformer.GetCorrespondingInputs(this->_input);
        const auto& newResetTrigger = transformer.GetCorrespondingInputs(this->_resetTrigger);
        const auto& newInputWeights1 = transformer.GetCorrespondingInputs(this->_inputWeights1);
        const auto& newInputWeights2 = transformer.GetCorrespondingInputs(this->_inputWeights2);
        const auto& newUpdateWeights1 = transformer.GetCorrespondingInputs(this->_updateWeights1);
        const auto& newUpdateWeights2 = transformer.GetCorrespondingInputs(this->_updateWeights2);
        const auto& newbiasGate = transformer.GetCorrespondingInputs(this->_biasGate);
        const auto& newbiasUpdate = transformer.GetCorrespondingInputs(this->_biasUpdate);
        const auto& newzeta = transformer.GetCorrespondingInputs(this->_zeta);
        const auto& newnu = transformer.GetCorrespondingInputs(this->_nu);
        auto newNode = transformer.AddNode<FastGRNNNode>(newInput, newResetTrigger, this->_hiddenUnits, this->_wRank, this->_uRank, newInputWeights1, newInputWeights2, newUpdateWeights1, newUpdateWeights2, newbiasGate, newbiasUpdate, newzeta, newnu, this->_gateActivation, this->_updateActivation);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ElementType>
    void FastGRNNNode<ElementType>::Define(FunctionDeclaration& fn)
    {
        fn.Define([this](const Value data, const Value reset, const Value inputWeights1, const Value inputWeights2, const Value updateWeights1, const Value updateWeights2, const Value biasGate, const Value biasUpdate, const Value zetaValue, const Value nuValue, Value result) {
            int hiddenUnits = static_cast<int>(this->_hiddenUnits);
            int wrank = static_cast<int>(this->_wRank);
            int urank = static_cast<int>(this->_uRank);
            _hiddenState = StaticAllocate("hiddenState", GetValueType<ElementType>(), MemoryLayout({ hiddenUnits }));
            _lastResetValue = StaticAllocate("lastResetValue", ValueType::Int32, ScalarLayout);

            int inputSize = static_cast<int>(input.Size());

            // zt = sigma( W x + U h + b_g)
            // ht1 = tanh( W x + U h + b_u )
            // ht = (sigma(zeta) (1 - zt) + sigma(nu)) ht1 + zt h

            // flatten the MemoryLayout so we can accept any shaped input data and produce any shape result.
            Vector input = ToVector(data);
            Vector resetVector = ToVector(reset);
            Matrix W1, W2, U1, U2;
            if (wrank == 0)
            {
                W1 = ToMatrix(inputWeights1, hiddenUnits, inputSize);
            }
            else
            {
                W1 = ToMatrix(inputWeights1, wrank, inputSize);
                W2 = ToMatrix(inputWeights2, hiddenUnits, wrank);
            }
            if (urank == 0)
            {
                U1 = ToMatrix(updateWeights1, hiddenUnits, hiddenUnits);
            }
            else
            {
                U1 = ToMatrix(updateWeights1, urank, hiddenUnits);
                U2 = ToMatrix(updateWeights2, hiddenUnits, urank);
            }
            Vector biasGateVector = ToVector(biasGate);
            Vector biasUpdateVector = ToVector(biasUpdate);
            Vector zetaVector = ToVector(zetaValue);
            Vector nuVector = ToVector(nuValue);
            Vector output = ToVector(result);

            // W * x + U *h
            // if we need to transpose W or U, we should do that in the importer so it is not done at runtime.
            Vector wxuh;
            if ((wrank != 0) && (urank != 0))
            {
                wxuh = GEMV(W2, GEMV(W1, input)) + GEMV(U2, GEMV(U1, _hiddenState));
            }
            else if ((wrank == 0) && (urank != 0))
            {
                wxuh = GEMV(W1, input) + GEMV(U2, GEMV(U1, _hiddenState));
            }
            else if ((wrank != 0) && (urank == 0))
            {
                wxuh = GEMV(W2, GEMV(W1, input)) + GEMV(U1, _hiddenState);
            }
            else
            {
                wxuh = GEMV(W1, input) + GEMV(U1, _hiddenState);
            }

            Vector zt = wxuh + biasGateVector;

            Vector ht1 = wxuh + biasUpdateVector;

            // Apply the activations.
            this->_gateActivation.Apply(zt);

            this->_updateActivation.Apply(ht1);

            Scalar zeta = zetaVector[0];
            Scalar nu = nuVector[0];

            // ht = (zeta.(1 - zt) + nu).ht1 + zt h
            //    = zeta.(1 - zt).ht1 + nu.ht1 + zt.h
            //    = (zeta.ht1) - (zeta.zt.ht1) + nu.ht1 + zt.h
            //    = (zeta + nu) ht1 - (zeta.zt.ht1) + (zt.h)
            Vector wu = zt * ht1;
            Scalar znu = zeta.Copy() + nu;
            Vector ht = (ht1 * znu) - (wu * zeta) + (zt * _hiddenState);

            this->_hiddenState = ht;

            if (resetVector.Size() > 0)
            {
                Scalar triggerValue = Cast<int>(resetVector[0]);

                If(triggerValue == 0, [&] {
                    // todo logical And operation present in ScalarOperations.h
                    // bugbug: logical AND is apparently not implemented in the value library...
                    If(_lastResetValue != 0, [&] {
                        // reset input value transitioned from 1 to 0, which is our reset trigger.
                        ClearHiddenState();
                    });
                });
                _lastResetValue = triggerValue;
            }

            // copy to output.
            output = ht;
        });
    }

    template <typename ElementType>
    void FastGRNNNode<ElementType>::DefineReset(FunctionDeclaration& fn)
    {
        fn.Define([this] {
            ClearHiddenState();
        });
    }

    template <typename ElementType>
    void FastGRNNNode<ElementType>::ClearHiddenState()
    {
        // bugbug: how to emit a "memset" operation here instead?
        Scalar zero(static_cast<ElementType>(0));
        For(_hiddenState, [this, zero](Scalar index) {
            _hiddenState[index] = zero;
        });
    }

    template <typename ElementType>
    void FastGRNNNode<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        model::CompilableNode::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver[resetTriggerPortName] << _resetTrigger;
        archiver["hiddenUnits"] << _hiddenUnits;
        archiver["wRank"] << _wRank;
        archiver["uRank"] << _uRank;
        archiver[W1PortName] << _inputWeights1;
        archiver[W2PortName] << _inputWeights2;
        archiver[U1PortName] << _updateWeights1;
        archiver[U2PortName] << _updateWeights2;
        archiver[biasGatePortName] << _biasGate;
        archiver[biasUpdatePortName] << _biasUpdate;
        archiver[zetaPortName] << _zeta;
        archiver[nuPortName] << _nu;

        _gateActivation.WriteToArchive(archiver);
        _updateActivation.WriteToArchive(archiver);
    }

    template <typename ElementType>
    void FastGRNNNode<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        model::CompilableNode::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver[resetTriggerPortName] >> _resetTrigger;
        archiver["hiddenUnits"] >> _hiddenUnits;
        archiver["wRank"] >> _wRank;
        archiver["uRank"] >> _uRank;
        archiver[W1PortName] >> _inputWeights1;
        archiver[W2PortName] >> _inputWeights2;
        archiver[U1PortName] >> _updateWeights1;
        archiver[U2PortName] >> _updateWeights2;
        archiver[biasGatePortName] >> _biasGate;
        archiver[biasUpdatePortName] >> _biasUpdate;
        archiver[zetaPortName] >> _zeta;
        archiver[nuPortName] >> _nu;

        _gateActivation.ReadFromArchive(archiver);
        _updateActivation.ReadFromArchive(archiver);

        this->_output.SetSize(_hiddenUnits);
    }

    // Explicit instantiations
    template class FastGRNNNode<float>;
    template class FastGRNNNode<double>;
} // namespace nodes
} // namespace ell
