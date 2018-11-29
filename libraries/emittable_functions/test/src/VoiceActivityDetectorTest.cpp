////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VoiceActivityDetectorTest.cpp (dsp)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "VoiceActivityDetectorTest.h"

#include <common/include/DataLoaders.h>

#include <data/include/Dataset.h>

#include <emittable_functions/include/VoiceActivityDetector.h>

#include <testing/include/testing.h>

#include <utilities/include/Files.h>
#include <utilities/include/JsonArchiver.h>
#include <utilities/include/StringUtil.h>

#include <value/include/ComputeContext.h>
#include <value/include/EmitterContext.h>

#include <algorithm>
#include <iostream>
#include <type_traits>
#include <vector>

using namespace ell;
using namespace ell::emitters;
using namespace ell::utilities;
using namespace ell::value;
using namespace ell::emittable_functions;

namespace ell
{

template <typename ValueType>
void TestVoiceActivityDetectorInternal(const std::string& filename, VoiceActivityDetector& vad, int frameSize)
{
    vad.BeginCompile();

    std::vector<ValueType> data(40);
    auto valueType = GetValueType<typename decltype(data)::value_type>();
    Vector temp = data;

    auto vadfn = CreateFunction("ProcessTest",
                                Value{ GetValueType<int>(), ScalarLayout },
                                { Value{ valueType, MemoryLayout({ (int)data.size() }) } },
                                [&vad](Vector data) -> Scalar { return vad.Process(data); });

    int errors = 0;
    // load the dataset
    auto stream2 = utilities::OpenIfstream(filename);
    int frame = 0;
    data::AutoSupervisedExampleIterator exampleIterator = ell::common::GetAutoSupervisedExampleIterator(stream2);
    while (exampleIterator.IsValid())
    {
        auto example = exampleIterator.Get();
        std::vector<double> data = example.GetDataVector().ToArray();
        std::vector<ValueType> buffer;
        std::transform(data.begin(), data.end(), std::back_inserter(buffer), [](double x) {
            return static_cast<ValueType>(x);
        });
        if (buffer.size() < static_cast<size_t>(frameSize))
        {
            buffer.resize(frameSize); // fix AutoDataVector possible compression
        }

        Scalar signal = vadfn(Vector(buffer));

        InvokeForContext<ComputeContext>([&](auto&) {
            If(signal != Cast<int>(example.GetMetadata().label), [&] {
                std::cout << "### Error on line " << frame << "\n";
                ++errors;
            });
        });
        frame++;
        exampleIterator.Next();
    }

    InvokeForContext<ComputeContext>([&](auto&) {
        testing::ProcessTest(FormatString("Testing %s", typeid(VoiceActivityDetector).name()), errors == 0);
    });
}

template <typename ValueType>
void TestVoiceActivityDetector(const std::string& path)
{
    const int FrameSize = 40;
    const double tauUp = 1.54;
    const double tauDown = 0.074326;
    const double largeInput = 2.400160;
    const double gainAtt = 0.002885;
    const double thresholdUp = 3.552713;
    const double thresholdDown = 0.931252;
    const double levelThreshold = 0.007885;

    VoiceActivityDetector
        vad(8000, FrameSize, 0.032, tauUp, tauDown, largeInput, gainAtt, thresholdUp, thresholdDown, levelThreshold);

    std::string filename = utilities::JoinPaths(path, { "..", "VadData.txt" });
    if (!utilities::FileExists(filename))
    {
        filename = utilities::JoinPaths(path, { "VadData.txt" });
    }

    TestVoiceActivityDetectorInternal<ValueType>(filename, vad, FrameSize);

    // test serialization

    SerializationContext context;
    {
        std::stringstream strstream;
        {
            JsonArchiver archiver(strstream);
            archiver.Archive("vad", vad);
        }

        JsonUnarchiver unarchiver(strstream, context);
        VoiceActivityDetector vad2;
        unarchiver.Unarchive("vad", vad2);

        testing::ProcessTest(FormatString("Deserialize %s", typeid(VoiceActivityDetector).name()), vad.Equals(vad2));

        TestVoiceActivityDetectorInternal<ValueType>(filename, vad2, FrameSize);
    }
}

//
// Explicit instantiations
//
template void TestVoiceActivityDetector<float>(const std::string& path);
template void TestVoiceActivityDetector<double>(const std::string& path);

} // namespace ell
