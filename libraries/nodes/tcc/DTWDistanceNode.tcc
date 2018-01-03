////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DTWDistanceNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <limits>

namespace ell
{
namespace nodes
{
    namespace DTWDistanceNodeImpl
    {
        template <typename ValueType>
        double Variance(const std::vector<std::vector<ValueType>>& prototype)
        {
            double sum = 0; // sum(x)
            double sumSquares = 0; // sum(x^2)
            size_t size = 0;
            for (const auto& vec : prototype)
            {
                size += vec.size();
                for (auto x : vec)
                {
                    sum += x;
                    sumSquares += (x * x);
                }
            }
            return (sumSquares - ((sum * sum) / size)) / size;
        }
    }

    template <typename ValueType>
    DTWDistanceNode<ValueType>::DTWDistanceNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 1), _sampleDimension(0), _prototypeLength(0), _prototypeVariance(0)
    {
    }

    template <typename ValueType>
    DTWDistanceNode<ValueType>::DTWDistanceNode(const model::PortElements<ValueType>& input, const std::vector<std::vector<ValueType>>& prototype)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, 1), _prototype(prototype)
    {
        _sampleDimension = input.Size();
        _prototypeLength = prototype.size();
        _d.resize(_prototypeLength + 1);
        _s.resize(_prototypeLength + 1);

        _prototypeVariance = DTWDistanceNodeImpl::Variance(_prototype);
        // _threshold = std::sqrt(-2 * std::log(confidenceThreshold)) * _prototypeVariance;
        Reset();
    }

    template <typename ValueType>
    void DTWDistanceNode<ValueType>::Reset() const
    {
        std::fill(_d.begin() + 1, _d.end(), std::numeric_limits<ValueType>::max());
        _d[0] = 0.0;
        std::fill(_s.begin(), _s.end(), 0);
        _currentTime = 0;
    }

    template <typename T>
    float distance(const std::vector<T>& a, const std::vector<T>& b)
    {
        T s = 0;
        for (size_t index = 0; index < a.size(); index++)
        {
            s += std::abs(a[index] - b[index]);
        }
        return static_cast<float>(s);
    }

    template <typename ValueType>
    void DTWDistanceNode<ValueType>::Compute() const
    {
        std::vector<ValueType> input = _input.GetValue();
        auto t = ++_currentTime;
        auto dLast = _d[0] = 0;
        auto sLast = _s[0] = t;

        ValueType bestDist = 0;
        int bestStart = 0;
        for (size_t index = 1; index < _prototypeLength + 1; ++index)
        {
            auto d_iMinus1 = _d[index - 1];
            auto dPrev_iMinus1 = dLast;
            auto dPrev_i = _d[index];
            auto s_iMinus1 = _s[index - 1];
            auto sPrev_iMinus1 = sLast;
            auto sPrev_i = _s[index];

            bestDist = d_iMinus1;
            bestStart = s_iMinus1;
            if (dPrev_i < bestDist)
            {
                bestDist = dPrev_i;
                bestStart = sPrev_i;
            }
            if (dPrev_iMinus1 < bestDist)
            {
                bestDist = dPrev_iMinus1;
                bestStart = sPrev_iMinus1;
            }
            bestDist += distance(_prototype[index - 1], input);

            _d[index] = bestDist;
            _s[index] = bestStart;
        }
        assert(bestDist == _d[_prototypeLength]);
        assert(bestStart == _s[_prototypeLength]);
        auto result = bestDist / _prototypeVariance;

        // Ensure best match is between 80% and 120% of prototype length
        auto timeDiff = _currentTime - bestStart;
        if (timeDiff < _prototypeLength * 0.8 || timeDiff > _prototypeLength * 1.2)
        {
            bestDist = std::numeric_limits<ValueType>::max();
        }

        _output.SetOutput({ static_cast<ValueType>(result) });
    };

    template <typename ValueType>
    void DTWDistanceNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newinput = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<DTWDistanceNode<ValueType>>(newinput, _prototype);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    std::vector<ValueType> DTWDistanceNode<ValueType>::GetPrototypeData() const
    {
        std::vector<ValueType> result;
        result.reserve(_prototypeLength * _sampleDimension);

        for (const auto& vec : _prototype)
        {
            result.insert(result.end(), vec.begin(), vec.end());
        }
        return result;
    }

    template <typename ValueType>
    void DTWDistanceNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        static_assert(!std::is_same<ValueType, bool>(), "Cannot instantiate boolean DTW nodes");

        auto inputType = GetPortVariableType(input);
        assert(inputType == GetPortVariableType(output));
        VerifyIsScalar(output);

        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        llvm::Value* pResult = compiler.EnsurePortEmitted(output);

        // The prototype (constant)
        emitters::Variable* pVarPrototype = function.GetModule().Variables().AddVariable<emitters::LiteralVectorVariable<ValueType>>(GetPrototypeData());

        // Global variables for the dynamic programming memory
        emitters::Variable* pVarD = function.GetModule().Variables().AddVariable<emitters::InitializedVectorVariable<ValueType>>(emitters::VariableScope::global, _prototypeLength + 1);

        // get global state vars
        llvm::Value* pPrototypeVector = function.GetModule().EnsureEmitted(*pVarPrototype);
        llvm::Value* pD = function.GetModule().EnsureEmitted(*pVarD);

        // incorrect usage of function.Variable --- should use IRModuleEmitter::EmitX(variable)
        llvm::Value* dist = function.Variable(inputType, "dist");
        llvm::Value* protoIndex = function.Variable(ell::emitters::VariableType::Int32, "i");
        llvm::Value* dLast = function.Variable(inputType, "dLast");
        llvm::Value* bestDist = function.Variable(inputType, "bestDist");

        // initialize variables
        function.StoreZero(protoIndex);
        function.StoreZero(dLast);

        auto forLoop = function.ForLoop();
        forLoop.Begin(_prototypeLength);
        {
            auto iMinusOne = forLoop.LoadIterationVariable();
            auto i = function.Operator(emitters::TypedOperator::add, iMinusOne, function.Literal(1));

            auto d_iMinus1 = function.ValueAt(pD, iMinusOne);
            auto dPrev_iMinus1 = function.Load(dLast);
            auto dPrev_i = function.ValueAt(pD, i);

            function.Store(bestDist, d_iMinus1);
            emitters::IRIfEmitter if1 = function.If(emitters::TypedComparison::lessThanFloat, dPrev_i, d_iMinus1);
            {
                function.Store(bestDist, dPrev_i);
            }
            if1.End();

            emitters::IRIfEmitter if2 = function.If(emitters::TypedComparison::lessThanFloat, dPrev_iMinus1, function.Load(bestDist));
            {
                function.Store(bestDist, dPrev_iMinus1);
            }
            if2.End();

            // Get dist
            function.StoreZero(dist);
            auto diffLoop = function.ForLoop();
            diffLoop.Begin(_sampleDimension);
            {
                auto j = diffLoop.LoadIterationVariable();
                llvm::Value* inputValue = function.ValueAt(pInput, j);
                llvm::Value* protoValue = function.ValueAt(pPrototypeVector, function.Load(protoIndex));
                llvm::Value* diff = function.Operator(emitters::GetSubtractForValueType<ValueType>(), inputValue, protoValue);
                llvm::Value* absDiff = function.Call(function.GetModule().GetRuntime().GetAbsFunction<ValueType>(), { diff });
                function.OperationAndUpdate(dist, emitters::GetAddForValueType<ValueType>(), absDiff);
                function.OperationAndUpdate(protoIndex, emitters::TypedOperator::add, function.Literal(1));
            }
            diffLoop.End();

            function.OperationAndUpdate(bestDist, emitters::GetAddForValueType<ValueType>(), function.Load(dist)); // x += dist;
            function.SetValueAt(pD, i, function.Load(bestDist)); // d[i] = x;
        }
        forLoop.End();

        function.Store(pResult, function.Operator(emitters::GetDivideForValueType<ValueType>(), function.Load(bestDist), function.Literal(static_cast<ValueType>(_prototypeVariance))));
    }

    template <typename ValueType>
    void DTWDistanceNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver[defaultOutputPortName] << _output;
        // archiver["prototype"] << _prototype;
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template <typename ValueType>
    void DTWDistanceNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver[defaultOutputPortName] >> _output;
        // archiver["prototype"] >> _prototype;
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }
}
}
