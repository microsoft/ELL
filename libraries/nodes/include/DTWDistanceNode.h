////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DTWDistanceNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SumNode.h"

#include "BinaryOperationNode.h"

#include <emitters/include/IRMath.h>

#include <model/include/CompilableNode.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/InputPort.h>
#include <model/include/MapCompiler.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>
#include <model/include/OutputPort.h>

#include <utilities/include/Exception.h>
#include <utilities/include/TypeName.h>

#include <string>

namespace ell
{
namespace nodes
{
    /// <summary> A node that computes the dynamic time-warping distance between its inputs </summary>
    template <typename ValueType>
    class DTWDistanceNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        DTWDistanceNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The signals to compare to the prototype </param>
        /// <param name="prototype"> The prototype </param>
        DTWDistanceNode(const model::OutputPort<ValueType>& input, const std::vector<std::vector<ValueType>>& prototype);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("DTWDistanceNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary></summary>
        std::vector<std::vector<ValueType>> GetPrototype() const { return _prototype; }

        /// <summary> Reset the state of the node </summary>
        void Reset() override;

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        bool HasState() const override { return true; }
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        std::vector<ValueType> GetPrototypeData() const;

        model::InputPort<ValueType> _input;
        model::OutputPort<ValueType> _output;

        size_t _sampleDimension;
        size_t _prototypeLength;
        std::vector<std::vector<ValueType>> _prototype;
        // double _threshold;
        double _prototypeVariance;

        mutable std::vector<ValueType> _d;
        mutable std::vector<int> _s;
        mutable int _currentTime;
    };
} // namespace nodes
} // namespace ell

#pragma region implementation

#include <emitters/include/IRLocalScalar.h>

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
    } // namespace DTWDistanceNodeImpl

    template <typename ValueType>
    DTWDistanceNode<ValueType>::DTWDistanceNode() :
        CompilableNode({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 1),
        _sampleDimension(0),
        _prototypeLength(0),
        _prototypeVariance(0)
    {
    }

    template <typename ValueType>
    DTWDistanceNode<ValueType>::DTWDistanceNode(const model::OutputPort<ValueType>& input, const std::vector<std::vector<ValueType>>& prototype) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, 1),
        _prototype(prototype)
    {
        // _threshold = std::sqrt(-2 * std::log(confidenceThreshold)) * _prototypeVariance;
        Reset();
    }

    template <typename ValueType>
    void DTWDistanceNode<ValueType>::Reset()
    {
        _sampleDimension = _input.Size();
        _prototypeLength = _prototype.size();
        _d.resize(_prototypeLength + 1);
        _s.resize(_prototypeLength + 1);

        _prototypeVariance = DTWDistanceNodeImpl::Variance(_prototype);
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
        const auto& newinput = transformer.GetCorrespondingInputs(_input);
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

        auto inputType = GetPortVariableType(_input);
        assert(inputType == GetPortVariableType(_output));
        VerifyIsScalar(_output);

        auto input = function.LocalArray(compiler.EnsurePortEmitted(_input));
        auto result = compiler.EnsurePortEmitted(_output);

        // The prototype (constant)
        emitters::Variable* pVarPrototype = function.GetModule().Variables().AddVariable<emitters::LiteralVectorVariable<ValueType>>(GetPrototypeData());

        // Global variables for the dynamic programming memory
        emitters::Variable* pVarD = function.GetModule().Variables().AddVariable<emitters::InitializedVectorVariable<ValueType>>(emitters::VariableScope::global, _prototypeLength + 1);

        // get global state vars
        auto prototypeVector = function.LocalArray(function.GetModule().EnsureEmitted(*pVarPrototype));
        auto pD = function.LocalArray(function.GetModule().EnsureEmitted(*pVarD));

        // incorrect usage of function.Variable --- should use IRModuleEmitter::EmitX(variable)
        auto dist = function.Variable(inputType, "dist");
        auto protoIndex = function.Variable(emitters::VariableType::Int32, "i");
        auto dLast = function.Variable(inputType, "dLast");
        auto bestDist = function.Variable(inputType, "bestDist");

        // initialize variables
        function.StoreZero(protoIndex);
        function.StoreZero(dLast);

        function.For(_prototypeLength, [pD, dLast, bestDist, dist, protoIndex, input, prototypeVector, this](emitters::IRFunctionEmitter& function, emitters::IRLocalScalar iMinusOne) {
            auto i = iMinusOne + 1;
            auto d_iMinus1 = pD[iMinusOne];
            auto dPrev_iMinus1 = function.LocalScalar(function.Load(dLast));
            auto dPrev_i = pD[i];

            function.Store(bestDist, static_cast<emitters::IRLocalScalar>(d_iMinus1));

            function.If(dPrev_i < d_iMinus1, [bestDist, dPrev_i](auto& function) {
                function.Store(bestDist, static_cast<emitters::IRLocalScalar>(dPrev_i));
            });

            function.If(dPrev_iMinus1 < function.Load(bestDist), [bestDist, dPrev_iMinus1](auto& function) {
                function.Store(bestDist, dPrev_iMinus1);
            });

            // Get dist
            function.StoreZero(dist);
            function.For(_sampleDimension, [dist, protoIndex, input, prototypeVector](emitters::IRFunctionEmitter& function, auto j) {
                auto inputValue = input[j];
                auto protoValue = prototypeVector[function.LocalScalar(function.Load(protoIndex))];
                auto absDiff = emitters::Abs(inputValue - protoValue);
                function.OperationAndUpdate(dist, emitters::GetAddForValueType<ValueType>(), absDiff);
                function.OperationAndUpdate(protoIndex, emitters::TypedOperator::add, function.Literal(1));
            });

            function.OperationAndUpdate(bestDist, emitters::GetAddForValueType<ValueType>(), function.Load(dist)); // x += dist;
            pD[i] = function.Load(bestDist); // d[i] = x;
        });

        function.Store(result, function.Load(bestDist) / function.LocalScalar<ValueType>(_prototypeVariance));
    }

    template <typename ValueType>
    void DTWDistanceNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver[defaultOutputPortName] << _output;
        // Since we know the prototype  will always be rectangular, we
        // archive it as a matrix here.
        auto numRows = _prototype.size();
        auto numColumns = _prototype[0].size();
        std::vector<double> elements;
        elements.reserve(numRows * numColumns);
        for (const auto& row : _prototype)
        {
            elements.insert(elements.end(), row.begin(), row.end());
        }
        archiver["prototype_rows"] << numRows;
        archiver["prototype_columns"] << numColumns;
        math::Matrix<double, math::MatrixLayout::columnMajor> temp(numRows, numColumns, elements);
        math::MatrixArchiver::Write(temp, "prototype", archiver);
    }

    template <typename ValueType>
    void DTWDistanceNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver[defaultOutputPortName] >> _output;
        size_t numRows;
        size_t numColumns;
        archiver["prototype_rows"] >> numRows;
        archiver["prototype_columns"] >> numColumns;
        math::Matrix<ValueType, math::MatrixLayout::columnMajor> temp(numRows, numColumns);
        math::MatrixArchiver::Read(temp, "prototype", archiver);
        for (size_t i = 0; i < numRows; i++)
        {
            _prototype.emplace_back(temp.GetRow(i).ToArray());
        }
        Reset();
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
