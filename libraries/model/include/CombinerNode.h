// ////////////////////////////////////////////////////////////////////////////////////////////////////
// //
// //  Project:  Embedded Machine Learning Library (EMLL)
// //  File:     CombinerNode.h (model)
// //  Authors:  Chuck Jacobs
// //
// ////////////////////////////////////////////////////////////////////////////////////////////////////

// #pragma once

// #include "Node.h"
// #include "InputPort.h"
// #include "OutputPort.h"

// #include <vector>
// #include <memory>
// #include <exception>

// /// <summary> model namespace </summary>
// namespace model
// {
//     template <typename ValueType>
//     class OutputRange
//     {
//     public:
//         OutputRange(const OutputPort<ValueType>& port);
//         OutputRange(const OutputPort<ValueType>& port, size_t index);
//         OutputRange(const OutputPort<ValueType>& port, size_t index, size_t numValues);

//         const OutputPort<ValueType>& port;
//         size_t startIndex;
//         size_t numValues;
//     };

//     template <typename ValueType>
//     using OutputRangeList = std::vector<OutputRange<ValueType>>;

//     /// <summary> An node that selects values from among a number of inputs </summary>
//     template <typename ValueType>
//     class CombinerNode : public Node
//     {
//     public:
//         /// <summary> Constructor </summary>
//         ///
//         /// <param name="inputs"> A vector of port ranges that represent the inputs to this node </param>
//         CombinerNode(const OutputRangeList<ValueType>& inputs);

//         CombinerNode(const OutputRange<ValueType>& input);

//         /// <summary> Gets the name of this type (for serialization). </summary>
//         ///
//         /// <returns> The name of this type. </returns>
//         static std::string GetTypeName() { return "Combiner"; }

//         /// <summary> Gets the name of this type (for serialization). </summary>
//         ///
//         /// <returns> The name of this type. </returns>
//         virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

//         /// <summary> Exposes the output port as a read-only property </summary>
//         const OutputPort<ValueType>& output = _output;

//     protected:
//         virtual void Compute() const override;

//     private:
//         struct InputRange
//         {
//             InputRange(Node* node, size_t portIndex, const OutputPort<ValueType>* refPort, size_t startIndex, size_t numValues) : port(node, portIndex, refPort), startIndex(startIndex), numValues(numValues)
//             {}

//             InputPort port;
//             size_t startIndex;
//             size_t numValues;
//         };

//         // Inputs
//         std::vector<InputRange> _inputRanges;

//         // Output
//         OutputPort<ValueType> _output;
//     };
// }

// #include "../tcc/CombinerNode.tcc"
