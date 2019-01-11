////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataUtils.cpp (utilities)
//  Authors:  Byron Changuion, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataUtils.h"

#include <common/include/DataLoaders.h>

#include <data/include/GeneralizedSparseParsingIterator.h>
#include <data/include/SequentialLineIterator.h>
#include <data/include/TextLine.h>
#include <data/include/WeightClassIndex.h>

#include <dsp/include/UnrolledConvolution.h>

#include <utilities/include/Files.h>

#include <algorithm>
#include <cmath>
#include <map>

using namespace ell;

// Utilities
namespace
{
// TODO: remove this eventually
using MultiClassDataset = ell::data::AutoSupervisedMultiClassDataset;
using BinaryDataset = ell::data::AutoSupervisedDataset;

void ThrowIfEmpty(const UnlabeledDataContainer& dataset)
{
    if (dataset.Size() == 0)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::badData, "Empty dataset");
    }
}

template <typename T1, typename T2>
void ThrowIfNotSameSize(const std::vector<T1>& a, const std::vector<T2>& b)
{
    if (a.size() != b.size())
    {
        throw utilities::InputException(utilities::InputExceptionErrors::badData, "Sizes don't match");
    }
}

template <typename T1, typename T2>
void ThrowIfNotSameSize(const math::RowVector<T1>& a, const math::RowVector<T2>& b)
{
    if (a.Size() != b.Size())
    {
        throw utilities::InputException(utilities::InputExceptionErrors::badData, "Sizes don't match");
    }
}

template <typename T1, typename T2>
math::RowVector<T1> CastVector(const math::RowVector<T2>& v)
{
    auto size = v.Size();
    math::RowVector<T1> result(size);
    for (size_t i = 0; i < size; ++i)
    {
        result[i] = v[i];
    }
    return result;
}

template <typename T1, typename T2>
std::vector<T1>& operator+=(std::vector<T1>& v, const std::vector<T2>& other)
{
    ThrowIfNotSameSize(v, other);
    for (size_t i = 0; i < other.size(); ++i)
    {
        v[i] += static_cast<T1>(other[i]);
    }
    return v;
}

template <typename T1, typename T2>
std::vector<T1>& operator-=(std::vector<T1>& v, const std::vector<T2>& other)
{
    ThrowIfNotSameSize(v, other);
    for (size_t i = 0; i < other.size(); ++i)
    {
        v[i] -= static_cast<T1>(other[i]);
    }
    return v;
}

template <typename T1, typename T2>
std::vector<T1>& operator*=(std::vector<T1>& v, const std::vector<T2>& other)
{
    ThrowIfNotSameSize(v, other);
    for (size_t i = 0; i < other.size(); ++i)
    {
        v[i] *= static_cast<T1>(other[i]);
    }
    return v;
}

template <typename T1, typename T2>
math::RowVector<T1>& operator*=(math::RowVector<T1>& v, const math::RowVector<T2>& other)
{
    ThrowIfNotSameSize(v, other);
    for (size_t i = 0; i < other.Size(); ++i)
    {
        v[i] *= static_cast<T1>(other[i]);
    }
    return v;
}

template <typename T1, typename T2>
std::vector<T1>& operator/=(std::vector<T1>& v, const std::vector<T2>& other)
{
    ThrowIfNotSameSize(v, other);
    for (size_t i = 0; i < other.size(); ++i)
    {
        auto rhs = other[i] == 0 ? 1.0 : other[i];
        v[i] /= static_cast<T1>(rhs);
    }
    return v;
}

template <typename T1, typename T2>
std::vector<T1>& operator/=(std::vector<T1>& v, T2 denom)
{
    denom = denom == 0 ? 1.0 : denom;

    for (size_t i = 0; i < v.size(); ++i)
    {
        v[i] /= static_cast<T1>(denom);
    }
    return v;
}

template <typename T1, typename T2>
math::RowVector<T1>& operator/=(math::RowVector<T1>& v, const math::RowVector<T2>& other)
{
    ThrowIfNotSameSize(v, other);
    for (size_t i = 0; i < other.Size(); ++i)
    {
        auto rhs = other[i] == 0 ? 1.0 : other[i];
        v[i] /= static_cast<T1>(rhs);
    }
    return v;
}

template <typename T1, typename T2>
math::RowVector<T1>& operator/=(math::RowVector<T1>& v, T2 denom)
{
    denom = denom == 0 ? 1.0 : denom;
    v.Transform([denom](auto x) { return x / denom; });
    return v;
}

template <typename T>
std::vector<T> operator-(std::vector<T>& a, const std::vector<T>& b)
{
    ThrowIfNotSameSize(a, b);
    auto v = a;
    v -= b;
    return v;
}

template <typename T>
math::RowVector<T> operator-(math::RowVector<T>& a, const math::RowVector<T>& b)
{
    ThrowIfNotSameSize(a, b);
    auto v = a;
    v -= b;
    return v;
}

template <typename T>
std::vector<T> operator*(const std::vector<T>& a, const std::vector<T>& b)
{
    ThrowIfNotSameSize(a, b);
    std::vector<T> v = a;
    v *= b;
    return v;
}

template <typename T>
math::RowVector<T> operator*(const math::RowVector<T>& a, const math::RowVector<T>& b)
{
    ThrowIfNotSameSize(a, b);
    auto v = a;
    v *= b;
    return v;
}

template <typename T>
std::vector<T> operator/(const std::vector<T>& a, double denom)
{
    auto v = a;
    v /= denom;
    return v;
}

template <typename T>
math::RowVector<T> operator/(const math::RowVector<T>& a, double denom)
{
    auto v = a;
    v /= denom;
    return v;
}

template <typename T>
std::vector<T> sqrt(const std::vector<T>& a)
{
    std::vector<T> result(a.size());
    std::transform(a.begin(), a.end(), result.begin(), [](const T& value) {
        return std::sqrt(value);
    });
    return result;
}

template <typename T>
math::RowVector<T> sqrt(const math::RowVector<T>& a)
{
    auto result = a;
    result.Transform([](auto x) { return std::sqrt(x); });
    return result;
}

template <typename ElementType>
math::ChannelColumnRowTensor<ElementType> GetImageTensor(const std::vector<ElementType>& imageFeatures, int numRows, int numColumns, int numChannels)
{
    math::ChannelColumnRowTensor<ElementType> result(numRows, numColumns, numChannels, imageFeatures);
    return result;
}

BinaryDataset LoadBinaryDataset(std::string filename)
{
    if (!utilities::IsFileReadable(filename))
    {
        throw utilities::SystemException(utilities::SystemExceptionErrors::fileNotFound, "Dataset file not readable: " + filename);
    }
    auto stream = utilities::OpenIfstream(filename);
    auto dataset = common::GetDataset(stream);
    return dataset;
}

BinaryLabelDataContainer FromDataset(const BinaryDataset& dataset)
{
    BinaryLabelDataContainer result;
    auto size = dataset.NumExamples();
    for (size_t i = 0; i < size; ++i)
    {
        const auto& example = dataset[i];
        auto dataVector = example.GetDataVector().ToArray();
        math::RowVector<float> newData(dataVector.size());
        for (size_t j = 0; j < dataVector.size(); ++j)
        {
            newData[j] = dataVector[j];
        }

        BinaryExample newExample{ newData, example.GetMetadata().label };
        result.push_back(newExample);
    }
    return result;
}

void VerifyAmountRead(std::istream& stream, int amount)
{
    if (stream.gcount() != amount)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
    }
}

int32_t ReadPortableInt32(std::istream& stream)
{
    uint32_t in = 0;
    stream.read(reinterpret_cast<char*>(&in), sizeof(in));
    uint8_t data[sizeof(in)] = {};
    memcpy(&data, &in, sizeof(data));

    return ((uint32_t)data[3] << 0) | ((uint32_t)data[2] << 8) | ((uint32_t)data[1] << 16) | ((uint32_t)data[0] << 24);
}

std::string GetDataFormat(std::string datasetFilename, std::string formatString)
{
    if (formatString.empty())
    {
        auto ext = utilities::GetFileExtension(datasetFilename, true);
        if (ext == "bin")
        {
            return "cifar";
        }
        else if (ext == "")
        {
            return "mnist";
        }

        return "gsdf";
    }

    return formatString;
}
} // namespace

// Prototypes
std::map<int, int> GetClassCounts(const MultiClassDataContainer& dataset);
template <typename DatasetType>
UnlabeledDataContainer GetDatasetInputs(const DatasetType& dataset);
void AppendCifarData(std::string cifarBatchFile, int maxRows, ell::math::ConstRowVectorReference<float> scale, ell::math::ConstRowVectorReference<float> biasAdjust, MultiClassDataContainer& dataset);

// Implementations
BinaryLabelDataContainer LoadBinaryLabelDataContainer(std::string filename)
{
    return FromDataset(LoadBinaryDataset(filename));
}

template <typename IndexValueParsingIterator>
struct RowVectorParser
{
    using type = math::RowVector<float>;
    static math::RowVector<float> Parse(ell::data::TextLine& textLine)
    {
        std::vector<float> vec;
        auto iter = IndexValueParsingIterator(textLine);
        while (iter.IsValid())
        {
            auto indexValue = iter.Get();
            if (vec.size() < indexValue.index + 1)
            {
                vec.resize(indexValue.index + 1);
            }
            vec[indexValue.index] = static_cast<float>(indexValue.value);
            iter.Next();
        }
        return { vec };
    }
};

MultiClassDataContainer LoadMultiClassDataContainer(std::string datasetFilename, std::string dataFormat, int maxRows)
{
    dataFormat = GetDataFormat(datasetFilename, dataFormat);
    if (dataFormat == "cifar")
    {
        return LoadCifarDataContainer(datasetFilename, maxRows);
    }
    else if (dataFormat == "mnist")
    {
        return LoadMnistDataContainer(datasetFilename, maxRows);
    }
    else // if (dataFormat == "gsdf")
    {
        auto dataset = LoadMultiClassDataContainer(datasetFilename, maxRows);
        return dataset;
    }
}

MultiClassDataContainer LoadMultiClassDataContainer(std::string filename, int maxRows)
{
    auto stream = utilities::OpenIfstream(filename);
    auto iter = common::GetExampleIterator<data::SequentialLineIterator, data::ClassIndexParser, RowVectorParser<data::GeneralizedSparseParsingIterator>>(stream);
    MultiClassDataContainer result;
    while (iter.IsValid() && (static_cast<int>(result.Size()) < maxRows || maxRows <= 0))
    {
        auto example = iter.Get();
        MultiClassExample newExample{ example.GetDataVector(), static_cast<int>(example.GetMetadata().classIndex) };
        result.Add(newExample);
        iter.Next();
    }
    return result;
}

MultiClassDataContainer LoadMnistDataContainer(std::string filename, int maxRows)
{
    auto labelFilename = filename + "-labels-idx1-ubyte";
    auto dataFilename = filename + "-images-idx3-ubyte";
    /*
All the integers in the files are stored in the MSB first (high endian) format used by most non-Intel processors. Users of Intel processors and other low-endian machines must flip the bytes of the header.

There are 4 files:

train-images-idx3-ubyte: training set images 
train-labels-idx1-ubyte: training set labels 
t10k-images-idx3-ubyte:  test set images 
t10k-labels-idx1-ubyte:  test set labels

The training set contains 60000 examples, and the test set 10000 examples.

The first 5000 examples of the test set are taken from the original NIST training set. The last 5000 are taken from the original NIST test set. The first 5000 are cleaner and easier than the last 5000.

TRAINING SET LABEL FILE (train-labels-idx1-ubyte):

[offset] [type]          [value]          [description] 
0000     32 bit integer  0x00000801(2049) magic number (MSB first) 
0004     32 bit integer  60000            number of items 
0008     unsigned byte   ??               label 
0009     unsigned byte   ??               label 
........ 
xxxx     unsigned byte   ??               label
The labels values are 0 to 9.

TRAINING SET IMAGE FILE (train-images-idx3-ubyte):

[offset] [type]          [value]          [description] 
0000     32 bit integer  0x00000803(2051) magic number 
0004     32 bit integer  60000            number of images 
0008     32 bit integer  28               number of rows 
0012     32 bit integer  28               number of columns 
0016     unsigned byte   ??               pixel 
0017     unsigned byte   ??               pixel 
........ 
xxxx     unsigned byte   ??               pixel
Pixels are organized row-wise. Pixel values are 0 to 255. 0 means background (white), 255 means foreground (black).
*/
    MultiClassDataContainer result;
    float biasAdjust = 1.0f;
    float scale = 2.0 / 256.0f;

    std::vector<char> labelData;
    std::vector<char> rawImageData;

    // read labels
    auto labelStream = utilities::OpenBinaryIfstream(labelFilename);
    int32_t labelMagicNumber = ReadPortableInt32(labelStream);
    if (labelMagicNumber != 0x0801)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::badData, "Bad magic number for MNIST label data file");
    }

    int numLabels = ReadPortableInt32(labelStream);
    if (maxRows > 0)
    {
        numLabels = std::min(numLabels, maxRows);
    }
    labelData.resize(numLabels);
    labelStream.read(labelData.data(), numLabels);
    VerifyAmountRead(labelStream, numLabels);

    auto dataStream = utilities::OpenBinaryIfstream(dataFilename);
    int32_t imageMagicNumber = ReadPortableInt32(dataStream);
    if (imageMagicNumber != 0x0803)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::badData, "Bad magic number for MNIST image data file");
    }

    int numImages = ReadPortableInt32(dataStream);
    if (maxRows > 0)
    {
        numImages = std::min(maxRows, numImages);
    }

    if (numImages != numLabels)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::badData, "MNIST label and image files have different sizes");
    }

    int numRows = ReadPortableInt32(dataStream);
    int numColumns = ReadPortableInt32(dataStream);
    int imageSize = numRows * numColumns;
    int imageDataSize = numRows * numColumns * numImages;
    rawImageData.resize(imageDataSize);
    dataStream.read(rawImageData.data(), imageDataSize);
    VerifyAmountRead(dataStream, imageDataSize);

    for (int i = 0; i < numImages; ++i)
    {
        math::RowVector<float> imageData(imageSize);
        for (int j = 0; j < imageSize; ++j)
        {
            auto val = (static_cast<unsigned char>(rawImageData[i * imageSize + j]) * scale) - biasAdjust;
            imageData[j] = val;
        }
        int label = static_cast<int>(static_cast<unsigned char>(labelData[i]));
        result.Add({ imageData, label });
    }

    return result;
}

// TODO: add order (row-maj / channel-maj) parameter
// TODO: add options for normalizing and/or mean-subtracting
MultiClassDataContainer LoadCifarDataContainer(std::string cifarBatchFile, int maxRows)
{
    MultiClassDataContainer result;
    // math::RowVector<float> biasAdjust({ 0.5f, 0.5f, 0.5f });
    // math::RowVector<float> scale({ 1.0f / 255, 1.0f / 255, 1.0f / 255 });
    math::RowVector<float> biasAdjust({ 0, 0, 0 });
    math::RowVector<float> scale({ 1, 1, 1 });
    AppendCifarData(cifarBatchFile, maxRows, scale, biasAdjust, result);
    return result;
}

void AppendCifarData(std::string cifarBatchFile, int maxRows, math::ConstRowVectorReference<float> scale, math::ConstRowVectorReference<float> biasAdjust, MultiClassDataContainer& dataset)
{
    // From docs:
    //
    // In other words, the first byte is the label of the first image, which is a
    // number in the range 0-9. The next 3072 bytes are the values of the pixels of the image.
    // The first 1024 bytes are the red channel values, the next 1024 the green, and the final
    // 1024 the blue.  The values are stored in row-major order, so the first 32 bytes are the
    // red channel values of the first row of the image.

    if (!utilities::IsFileReadable(cifarBatchFile))
    {
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Data file not readable.");
    }

    auto stream = utilities::OpenBinaryIfstream(cifarBatchFile);
    if (!stream.good())
    {
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Data file not readable.");
    }
    const int labelSize = 1;
    const int planeSize = 1024;
    char labelBuf[labelSize] = { 0 };
    char red[planeSize];
    char green[planeSize];
    char blue[planeSize];
    int count = 0;
    while (count < maxRows || maxRows <= 0)
    {
        stream.read(labelBuf, labelSize);
        stream.read(red, planeSize);
        stream.read(green, planeSize);
        stream.read(blue, planeSize);
        if (!stream.good() || stream.eof())
        {
            break;
        }
        ++count;

        math::RowVector<float> imageData(3 * planeSize);
        for (int i = 0; i < planeSize; ++i)
        {
            imageData[3 * i + 0] = (static_cast<unsigned char>(red[i]) * scale[0]) - biasAdjust[0];
            imageData[3 * i + 1] = (static_cast<unsigned char>(green[i]) * scale[1]) - biasAdjust[1];
            imageData[3 * i + 2] = (static_cast<unsigned char>(blue[i]) * scale[2]) - biasAdjust[2];
        }
        int label = static_cast<int>(static_cast<unsigned char>(labelBuf[0]));
        dataset.Add({ imageData, label });
    }
}

std::map<int, int> GetClassCounts(const MultiClassDataContainer& dataset)
{
    std::map<int, int> counts;
    auto size = dataset.Size();
    for (size_t i = 0; i < size; ++i)
    {
        const auto& example = dataset[i];
        int classIndex = example.output;
        counts[classIndex] += 1;
    }

    return counts;
}

int GetMaxClassIndex(const std::map<int, int>& counts)
{
    int maxIndex = -1;
    for (const auto& it : counts)
    {
        maxIndex = std::max(maxIndex, it.first);
    }
    return maxIndex;
}

int GetNumRows(const UnlabeledDataContainer& dataset)
{
    return dataset.Size();
}

int GetNumColumns(const UnlabeledDataContainer& dataset)
{
    ThrowIfEmpty(dataset);
    return dataset[0].Size();
}

int GetNumColumns(const UnlabeledDataContainer& dataset, const utilities::MemoryLayout& memoryLayout, int dimension)
{
    ThrowIfEmpty(dataset);
    if (dataset[0].Size() != memoryLayout.NumElements())
    {
        throw utilities::InputException(utilities::InputExceptionErrors::badData, "Dataset size doesn't match memory layout");
    }

    return memoryLayout.GetActiveSize()[dimension];
}

template <typename T>
std::vector<T> ToArray(const data::IDataVector& data, size_t size)
{
    std::vector<T> result;
    auto temp = data.ToArray(size);
    std::transform(temp.begin(), temp.end(), std::back_inserter(result), [](double x) { return static_cast<T>(x); });
    return result;
}

UnlabeledDataContainer GetImageMatrixDataset(const UnlabeledDataContainer& imageData, int numRows, int numColumns, int numChannels)
{
    auto datasetRowSize = static_cast<int>(imageData[0].Size());
    if (datasetRowSize != numRows * numColumns * numChannels)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input data rows not of size rows*columns*channels");
    }

    UnlabeledDataContainer trainingFeatures;
    for (const auto& imageFeatures : imageData)
    {
        // imageFeatures is a vector containing all the values for an image (in row-major order)
        // We convert that to a row-major matrix of size (rows*columns) x (channels), where each
        // row contains all the channel data for a single (r,c) location
        math::RowMatrix<float> imageMatrix(numRows * numColumns, numChannels, imageFeatures.ToArray());

        // Add each "pixel" entry in image matrix to result dataset
        for (int i = 0; i < numRows * numColumns; ++i)
        {
            trainingFeatures.Add(imageMatrix.GetRow(i).ToArray());
        }
    }

    return trainingFeatures;
}

// TODO: deal with padding
UnlabeledDataContainer GetUnrolledImageDataset(const UnlabeledDataContainer& imageData, int numRows, int numColumns, int numChannels, int filterSize, int stride)
{
    auto datasetRowSize = static_cast<int>(imageData[0].Size());
    if (datasetRowSize != numRows * numColumns * numChannels)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input data rows not of size rows*columns*channels");
    }

    const auto numOutputRows = (static_cast<int>(numRows) - filterSize + 1) / stride;
    const auto numOutputColumns = (static_cast<int>(numColumns) - filterSize + 1) / stride;

    math::RowMatrix<float> unrolledImageFeatureMatrix(filterSize * filterSize * numChannels, numOutputRows * numOutputColumns);
    UnlabeledDataContainer trainingFeatures;
    for (const auto& imageFeatures : imageData)
    {
        auto featureTensor = GetImageTensor(imageFeatures.ToArray(), numRows, numColumns, numChannels);
        dsp::ReceptiveFieldToColumns(featureTensor, filterSize, stride, unrolledImageFeatureMatrix);
        // unrolledImageFeatureMatrix is a (filterSize * filterSize * numChannels) x (rows * columns) matrix
        // Add each "pixel" entry in unrolledImageMatrix to trainingFeatures
        for (size_t i = 0; i < unrolledImageFeatureMatrix.NumColumns(); ++i)
        {
            trainingFeatures.Add(unrolledImageFeatureMatrix.GetColumn(i).ToArray());
        }
    }

    return trainingFeatures;
}

VectorLabelDataContainer CreateVectorLabelDataContainer(const UnlabeledDataContainer& features, const UnlabeledDataContainer& labels)
{
    VectorLabelDataContainer dataset;
    auto size = features.Size();
    for (size_t i = 0; i < size; ++i)
    {
        dataset.emplace_back(VectorLabelExample{ features[i], labels[i] });
    }
    return dataset;
}

UnlabeledDataContainer GetDatasetInputs(const BinaryLabelDataContainer& dataset)
{
    return GetDatasetInputs<BinaryLabelDataContainer>(dataset);
}

UnlabeledDataContainer GetDatasetInputs(const MultiClassDataContainer& dataset)
{
    return GetDatasetInputs<MultiClassDataContainer>(dataset);
}

UnlabeledDataContainer GetDatasetInputs(const VectorLabelDataContainer& dataset)
{
    return GetDatasetInputs<VectorLabelDataContainer>(dataset);
}

template <typename DatasetType>
UnlabeledDataContainer GetDatasetInputs(const DatasetType& dataset)
{
    UnlabeledDataContainer result;
    auto size = dataset.Size();
    for (size_t i = 0; i < size; ++i)
    {
        result.Add(dataset[i].input);
    }
    return result;
}

UnlabeledDataContainer GetDatasetOutputs(const VectorLabelDataContainer& dataset)
{
    UnlabeledDataContainer result;
    auto size = dataset.Size();
    for (size_t i = 0; i < size; ++i)
    {
        result.Add(dataset[i].output);
    }
    return result;
}

double GetModelAccuracy(const BinaryLabelDataContainer& dataset, const UnlabeledDataContainer& predictions)
{
    auto size = dataset.Size();
    if (predictions.Size() != size)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
    }

    int pos_count = 0;
    for (size_t i = 0; i < size; ++i)
    {
        auto pred = predictions[i][0];
        auto label = dataset[i].output;
        if (pred * label > 0)
        {
            ++pos_count;
        }
    }
    return static_cast<double>(pos_count) / size;
}

double GetModelAccuracy(const MultiClassDataContainer& dataset, const UnlabeledDataContainer& predictions)
{
    auto size = dataset.Size();
    if (predictions.Size() != size)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
    }

    int pos_count = 0;
    for (size_t i = 0; i < size; ++i)
    {
        const auto& v = predictions[i].ToArray();
        auto predIndex = static_cast<int>(std::distance(v.begin(), std::max_element(v.begin(), v.end())));
        auto classIndex = dataset[i].output;
        if (classIndex == predIndex)
        {
            ++pos_count;
        }
    }
    return static_cast<double>(pos_count) / size;
}
