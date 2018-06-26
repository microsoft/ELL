////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VoiceActivityDetectorNode.cpp (nodes)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "VoiceActivityDetectorNode.h"

// emitters
#include "EmitterException.h"
#include "EmitterTypes.h"
#include "IREmitter.h"
#include "IRLocalValue.h"

namespace ell
{
using namespace emitters;
namespace nodes
{

    using TickType = int64_t;

    template <typename ValueType>
    void VoiceActivityDetectorNode<ValueType>::Compute() const
    {
        int signal = _vad.process(_input.GetValue());
        _output.SetOutput({ signal });
    };

    template <typename ValueType>
    void VoiceActivityDetectorNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<VoiceActivityDetectorNode<ValueType>>(newPortElements, _vad.getSampleRate(),
            _vad.getFrameDuration(), _vad.getTauUp(), _vad.getTauDown(), _vad.getLargeInput(), _vad.getGainAtt(), _vad.getThresholdUp(), _vad.getThresholdDown(), _vad.getLevelThreshold());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void VoiceActivityDetectorNode<ValueType>::Compile(model::IRMapCompiler& compiler, IRFunctionEmitter& function)
    {
        auto tauUpLiteral = function.template Literal<ValueType>(_vad.getTauUp());
        auto tauDownLiteral = function.template Literal<ValueType>(_vad.getTauDown());
        auto largeInputLiteral = function.template Literal<ValueType>(_vad.getLargeInput());
        auto gainAttLiteral = function.template Literal<ValueType>(_vad.getGainAtt());
        auto thresholdUpLiteral = function.template Literal<ValueType>(_vad.getThresholdUp());
        auto thresholdDownLiteral = function.template Literal<ValueType>(_vad.getThresholdDown());
        auto levelThresholdLiteral = function.template Literal<ValueType>(_vad.getLevelThreshold());

        TypedOperator add = GetAddForValueType<ValueType>();
        TypedOperator addTick = GetAddForValueType<TickType>();
        TypedOperator subtract = GetSubtractForValueType<ValueType>();
        TypedOperator multiply = GetMultiplyForValueType<ValueType>();
        TypedOperator divide = GetDivideForValueType<ValueType>();
        auto lessThan = GetComparison<ValueType>(BinaryPredicateType::less);
        auto greaterThan = GetComparison<ValueType>(BinaryPredicateType::greater);

        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        const int inputSize = input.Size();
        if (inputSize != _vad.getWindowSize())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input size must match VoiceActivityDetectorNode windowSize");
        }

        // assumes ValueType is either float or double.
        auto windowSizeLiteral = function.template Literal<ValueType>(static_cast<ValueType>(_vad.getWindowSize()));
        auto frameDurationLiteral = function.template Literal<ValueType>(static_cast<ValueType>(_vad.getFrameDuration()));

        // Get LLVM types
        auto& module = function.GetModule();
        auto& emitter = function.GetEmitter();
        auto elementType = emitter.Type(GetVariableType<ValueType>());
        if (!llvm::VectorType::isValidElementType(elementType))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Invalid element type for LLVM vector");
        }

        // Allocate global variable to hold the weights lookup table, first convert the weights to the rigth ValueType.
        auto actualWeights = _vad.getWeights();
        std::vector<ValueType> weights;
        std::transform(actualWeights.begin(), actualWeights.end(), std::back_inserter(weights), [](double x) { return static_cast<ValueType>(x); });

        //// Allocate global variable to hold the computed CMessageWeights.
        llvm::GlobalVariable* gWeights = module.GlobalArray<ValueType>(compiler.GetGlobalName(*this, "weights"), weights);
        //// Allocate global variable to hold the time
        llvm::GlobalVariable* ticks = module.Global<TickType>(compiler.GetGlobalName(*this, "ticks"), 0);
        // Allocate global variable to hold the previous time
        llvm::GlobalVariable* lastTime = module.Global<ValueType>(compiler.GetGlobalName(*this, "lastTime"), 0);
        // Allocate global variable to hold the previous power level
        llvm::GlobalVariable* lastLevel = module.Global<ValueType>(compiler.GetGlobalName(*this, "lastLevel"), static_cast<ValueType>(0.1));
        // Allocate global variable to hold the previous signal (because sometimes this function does not change it).
        llvm::GlobalVariable* signal = module.Global<int>(compiler.GetGlobalName(*this, "signal"), 0);

        // Compute the weighted power level of the given input (the commented code is from our reference implementation, see VoiceActivityDetector.cpp).
        //double level = 0;
        //for (int i = 0; i < _impl->_windowSize; i++)
        //{
        //    level += data[i] * _impl->_cmw.getWeight(i);
        //}
        llvm::Value* level = function.Variable(elementType, "level");
        function.Store(level, function.Literal(static_cast<ValueType>(0.0)));

        function.For(inputSize, [pInput, gWeights, multiply, add, level](IRFunctionEmitter& fn, llvm::Value* index) {
            auto value = fn.ValueAt(pInput, index);
            auto w = fn.ValueAt(gWeights, index);
            fn.OperationAndUpdate(level, add, fn.Operator(multiply, value, w));
        });

        // level = level / (double)_impl->_windowSize
        function.OperationAndUpdate(level, divide, windowSizeLiteral);

        // double time = _impl->_time++ * _impl->_frameDuration;
        llvm::Value* time = function.Variable(elementType, "time");
        function.Store(time, function.CastValue<TickType, ValueType>(function.Load(ticks)));
        function.OperationAndUpdate(time, multiply, frameDurationLiteral);
        function.OperationAndUpdate(ticks, addTick, function.template Literal<TickType>(1)); // _time++

        // signal = _impl->_tracker.classify(t, level);
        //double timeDelta = time - this->_lastTime;
        llvm::Value* timeDelta = function.Variable(elementType, "timeDelta");
        function.Store(timeDelta, function.Load(time));
        function.OperationAndUpdate(timeDelta, subtract, function.Load(lastTime));

        //double levelDelta = level - this->_lastLevel;
        llvm::Value* levelDelta = function.Variable(elementType, "levelDelta");
        function.Store(levelDelta, function.Load(level));
        function.OperationAndUpdate(levelDelta, subtract, function.Load(lastLevel));

        // bugbug: the comparisons have to be done first outside the If/ElseIf/Else block otherwise we get weird compile errors.
        auto cmp1 = function.Comparison(lessThan, function.Load(level), function.Load(lastLevel));
        auto cmp2 = function.Comparison(greaterThan, function.Load(level), function.Operator(multiply, largeInputLiteral, function.Load(lastLevel)));

        // for the function.If "endOnDestruct" to happen at the right time, if1 needs to be inside it's own block
        {
            //if (level < this->_lastLevel)
            //{
            //    this->_lastLevel = this->_lastLevel + timeDelta / this->_tauDown * levelDelta;
            //    if (this->_lastLevel < level)
            //    {
            //        this->_lastLevel = level;
            //    }
            //}

            auto if1 = function.If(cmp1, [=](IRFunctionEmitter& fn) {
                fn.Store(lastLevel, fn.Operator(add, fn.Load(lastLevel), fn.Operator(multiply, fn.Operator(divide, fn.Load(timeDelta), tauDownLiteral), fn.Load(levelDelta))));
                fn.If(fn.Comparison(lessThan, fn.Load(lastLevel), fn.Load(level)), [lastLevel, level](IRFunctionEmitter& fn) {
                    fn.Store(lastLevel, fn.Load(level));
                });
            });
            //else if (level > this->_largeInput * this->_lastLevel)
            //{
            //    this->_lastLevel = this->_lastLevel + this->_gainAtt * timeDelta / this->_tauUp * levelDelta;
            //    if (this->_lastLevel > level)
            //    {
            //        this->_lastLevel = level;
            //    }
            //}
            if1.ElseIf(cmp2, [=](IRFunctionEmitter& fn) {
                fn.Store(lastLevel, fn.Operator(add, fn.Load(lastLevel), fn.Operator(multiply, gainAttLiteral, fn.Operator(multiply, fn.Operator(divide, fn.Load(timeDelta), tauUpLiteral), fn.Load(levelDelta)))));
                fn.If(fn.Comparison(greaterThan, fn.Load(lastLevel), fn.Load(level)), [lastLevel, level](IRFunctionEmitter& fn) {
                    fn.Store(lastLevel, fn.Load(level));
                });
            });
            //else
            //{
            //    this->_lastLevel = this->_lastLevel + timeDelta / this->_tauUp * levelDelta;
            //    if (this->_lastLevel > level)
            //    {
            //        this->_lastLevel = level;
            //    }
            //}
            if1.Else([=](IRFunctionEmitter& fn) {
                fn.Store(lastLevel, fn.Operator(add, fn.Load(lastLevel), fn.Operator(multiply, fn.Operator(divide, fn.Load(timeDelta), tauUpLiteral), fn.Load(levelDelta))));
                fn.If(fn.Comparison(greaterThan, fn.Load(lastLevel), fn.Load(level)), [lastLevel, level](IRFunctionEmitter& fn) {
                    fn.Store(lastLevel, fn.Load(level));
                });
            });
        }

        {
            //if ((level > this->_thresholdUp * this->_lastLevel) && (level > this->_levelThreshold))
            //{
            //    this->_signal = 1;
            //}
            function.If(function.LogicalAnd(
                            function.Comparison(greaterThan, function.Load(level), function.Operator(multiply, thresholdUpLiteral, function.Load(lastLevel))),
                            function.Comparison(greaterThan, function.Load(level), levelThresholdLiteral)),
                        [signal](IRFunctionEmitter& fn) {
                            fn.Store(signal, fn.Literal(1));
                        });
        }

        {
            //if (level < this->_thresholdDown * this->_lastLevel)
            //{
            //    this->_signal = 0;
            //}
            function.If(function.Comparison(lessThan, function.Load(level), function.Operator(multiply, thresholdDownLiteral, function.Load(lastLevel))), [signal](IRFunctionEmitter& fn) {
                fn.Store(signal, fn.Literal(0));
            });
        }

        //this->_lastTime = time;
        function.Store(lastTime, function.Load(time));
        // return this->_signal;

        // Set the output
        function.Store(pOutput, function.Load(signal));
    }

    template <typename ValueType>
    void VoiceActivityDetectorNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        CompilableNode::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["vad"] << _vad;
    }

    template <typename ValueType>
    void VoiceActivityDetectorNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        CompilableNode::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["vad"] >> _vad;
    }

    // Explicit specializations
    template class VoiceActivityDetectorNode<float>;
    template class VoiceActivityDetectorNode<double>;
} // nodes
} // ell
