////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoopNest_convolution_test.cpp (value)
//  Authors:  Mason Remy
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoopNest_convolution_test.h"
#include "TestUtil.h"

#include <value/include/CachingStrategies.h>
#include <value/include/ComputeContext.h>
#include <value/include/EmitterContext.h>
#include <value/include/FunctionDeclaration.h>
#include <value/include/LLVMContext.h>
#include <value/include/LoopNests.h>
#include <value/include/Matrix.h>
#include <value/include/Scalar.h>
#include <value/include/Tensor.h>
#include <value/include/Value.h>
#include <value/include/Vector.h>

#include <value/include/loopnests/CodeGenerator.h>
#include <value/include/loopnests/Kernel.h>
#include <value/include/loopnests/LoopNest.h>
#include <value/include/loopnests/LoopNestPrinter.h>

#include <emitters/include/IRFunctionEmitter.h>

#include <math/include/Matrix.h>
#include <math/include/Tensor.h>
#include <math/include/Vector.h>

#include <utilities/include/FunctionUtils.h>
#include <utilities/include/Logger.h>

#include <testing/include/testing.h>

#include <optional>
#include <tuple>
#include <vector>

using namespace ell::emitters;
using namespace ell::utilities;
using namespace ell::logging;
using namespace ell::value;
using namespace ell::value::loopnests;

namespace ell
{
// Tests of convolution via LoopNests

int GetOutputDimensionSize(int inputSize, int receptiveFieldSize, int stride, int paddingSize)
{
    return (inputSize + 2 * paddingSize - receptiveFieldSize) / stride + 1;
}

struct ConvolutionConfig
{
    ConvolutionConfig(const std::vector<int>& inputSizes,
                      int outputFilters,
                      const std::vector<int>& receptiveFieldSize,
                      const std::vector<int>& strideSize,
                      const std::vector<int>& paddingSize,
                      const std::vector<int>& inputBlockSizes,
                      const std::vector<int>& outputBlockSizes)
    {
        outputSize[2] = outputFilters;
        for (int dim = 0; dim < 3; dim++)
        {
            inputSize[dim] = inputSizes[dim];
            inputBlockSize[dim] = inputBlockSizes[dim];
            outputBlockSize[dim] = outputBlockSizes[dim];

            // Value that are only computed in the row/column dimensions
            if (dim < 2)
            {
                receptiveField[dim] = receptiveFieldSize[dim];
                stride[dim] = strideSize[dim];
                padding[dim] = paddingSize[dim];
                outputSize[dim] = GetOutputDimensionSize(inputSize[dim], receptiveFieldSize[dim], strideSize[dim], paddingSize[dim]);
            }

            if (inputBlockSize[dim] > 0)
            {
                inputBlockCount[dim] = inputSize[dim] / inputBlockSize[dim];
                if (inputSize[dim] % inputBlockSize[dim] != 0)
                {
                    inputBlockCount[dim]++;
                }
            }

            if (outputBlockSize[dim] > 0)
            {
                outputBlockCount[dim] = outputSize[dim] / outputBlockSize[dim];
                if (outputSize[dim] % outputBlockSize[dim] != 0)
                {
                    outputBlockCount[dim]++;
                }
            }
        }

        weightSize[0] = outputSize[2];
        weightSize[1] = inputSize[2];
        weightSize[2] = receptiveField[0];
        weightSize[3] = receptiveField[1];

        MemoryShape inputPackedShape = { inputBlockCount[2], inputSize[0], inputSize[1], inputBlockSize[2] };
        MemoryShape inputPackedPadding = { 0, padding[0], padding[1], 0 };
        inputPackedPaddedLayout = { inputPackedShape, inputPackedPadding };
        MemoryShape inputLogicalPadding = { padding[0], padding[1], 0 };
        inputLogicalPaddedLayout = { MemoryShape{ inputSize[0], inputSize[1], inputSize[2] }, inputLogicalPadding };

        outputPackedLayout = { MemoryShape{ outputBlockCount[2], outputSize[0], outputSize[1], outputBlockSize[2] } };
        outputLogicalLayout = { MemoryShape{ outputSize[0], outputSize[1], outputSize[2] } };

        weightPackedLayout = { MemoryShape{
            outputBlockCount[2],
            inputBlockCount[2],
            weightSize[2],
            weightSize[3],
            inputBlockSize[2],
            outputBlockSize[2] } };
    }

    int inputSize[3];
    int outputSize[3];
    int weightSize[4];
    int receptiveField[2];
    int stride[2];
    int padding[2];

    int inputBlockSize[3];
    int outputBlockSize[3];

    int inputBlockCount[3];
    int outputBlockCount[3];

    MemoryLayout inputPackedPaddedLayout;
    MemoryLayout inputLogicalPaddedLayout;

    MemoryLayout outputPackedLayout;
    MemoryLayout outputLogicalLayout;

    MemoryLayout weightPackedLayout;
};

Tensor NaiveForLoopConvolution(const ConvolutionConfig& config, Tensor input, Array weights)
{
    auto output = MakeTensor<int>(config.outputSize[0], config.outputSize[1], config.outputSize[2], "expectedOutput");
    ForRange(config.outputSize[2], [&](Scalar outputChannel) {
        ForRange(config.inputSize[2], [&](Scalar inputChannel) {
            ForRange(config.outputSize[0], [&](Scalar outputRow) {
                ForRange(config.outputSize[1], [&](Scalar outputColumn) {
                    ForRange(config.receptiveField[0], [&](Scalar weightRow) {
                        ForRange(config.receptiveField[1], [&](Scalar weightColumn) {
                            Scalar inputRow = outputRow * config.stride[0] + weightRow - config.padding[0];
                            Scalar inputColumn = outputColumn * config.stride[1] + weightColumn - config.padding[1];
                            If(inputRow >= 0, [&] {
                                If(inputRow < Scalar{ config.inputSize[0] }, [&] {
                                    If(inputColumn >= 0, [&] {
                                        If(inputColumn < Scalar{ config.inputSize[1] }, [&] {
                                            output(outputRow, outputColumn, outputChannel) +=
                                                input(inputRow, inputColumn, inputChannel) *
                                                weights({ outputChannel, inputChannel, weightRow, weightColumn });
                                        });
                                    });
                                });
                            });
                        });
                    });
                });
            });
        });
    });
    return output;
}

Tensor UnpackOutputTensor(const ConvolutionConfig& config, Value packedOutput)
{
    auto unpackedOutput = MakeTensor<int>(config.outputSize[0], config.outputSize[1], config.outputSize[2], "unpackedOutput");
    packedOutput.SetLayout(config.outputPackedLayout);
    auto packedOutputArray = Array(packedOutput);

    ForRange(config.outputSize[2], [&](Scalar channelIdx) {
        ForRange(config.outputSize[0], [&](Scalar rowIdx) {
            ForRange(config.outputSize[1], [&](Scalar columnIdx) {
                unpackedOutput(rowIdx, columnIdx, channelIdx) = packedOutputArray({ channelIdx / config.outputBlockSize[2],
                                                                                    rowIdx,
                                                                                    columnIdx,
                                                                                    channelIdx % config.outputBlockSize[2] });
            });
        });
    });
    return unpackedOutput;
}
} // namespace ell
