////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (apply)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ApplyArguments.h"

#include <utilities/include/CommandLineParser.h>
#include <utilities/include/Exception.h>
#include <utilities/include/Files.h>
#include <utilities/include/OutputStreamImpostor.h>

#include <data/include/DataVector.h>
#include <data/include/DataVectorOperations.h>
#include <data/include/Dataset.h>
#include <data/include/Example.h>

#include <math/include/Vector.h>
#include <math/include/VectorOperations.h>

#include <common/include/DataLoadArguments.h>
#include <common/include/DataLoaders.h>
#include <common/include/DataSaveArguments.h>
#include <common/include/LoadModel.h>
#include <common/include/MapLoadArguments.h>

#include <model/include/Map.h>
#include <model/include/OutputNode.h>

#include <iostream>
#include <stdexcept>
#include <string>

using namespace ell;

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        common::ParsedDataLoadArguments dataLoadArguments;
        common::ParsedDataSaveArguments dataSaveArguments;
        common::ParsedMapLoadArguments mapLoadArguments;
        ParsedApplyArguments applyArguments;

        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(dataSaveArguments);
        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(applyArguments);

        // parse command line
        commandLineParser.Parse();

        // load map
        auto map = common::LoadMap(mapLoadArguments);

        // get data iterator
        auto stream = utilities::OpenIfstream(dataLoadArguments.inputDataFilename);
        auto exampleIterator = common::GetAutoSupervisedExampleIterator(stream);

        // get output stream
        auto& outputStream = dataSaveArguments.outputDataStream;

        // output summarization mode
        if (applyArguments.summarize)
        {
            model::Map map2;
            if (applyArguments.inputMapFilename2 != "")
            {
                map2 = common::LoadMap(applyArguments.inputMapFilename2);
            }

            math::RowVector<double> u(map.GetOutputSize());
            math::RowVector<double> v(map.GetOutputSize());
            size_t count = 0;

            while (exampleIterator.IsValid())
            {
                auto example = exampleIterator.Get();
                auto mappedDataVector = map.Compute<data::DoubleDataVector>(example.GetDataVector());
                math::RowVector<double> w(map.GetOutputSize());
                w += mappedDataVector;

                auto mappedDataVector2 = map2.Compute<data::DoubleDataVector>(example.GetDataVector());
                w += (-1.0) * mappedDataVector2;

                // accumulate vectors for mean and standard deviation computation
                u += w;
                w.Transform([](double x) { return x * x; });
                v += w;

                exampleIterator.Next();
                ++count;
            }

            // calculate and print mean and standard deviation
            double denominator = static_cast<double>(count);
            u /= denominator;
            outputStream << "mean:\t" << u << '\n';

            u.Transform([](double x) { return x * x; });
            v /= denominator;
            v -= u;
            v.Transform([](double x) { return std::sqrt(x); });
            outputStream << "std:\t" << v << '\n';
        }

        // output new dataset mode
        else
        {
            while (exampleIterator.IsValid())
            {
                auto example = exampleIterator.Get();
                auto mappedDataVector = map.Compute<data::FloatDataVector>(example.GetDataVector());
                auto mappedExample = data::DenseSupervisedExample(std::move(mappedDataVector), example.GetMetadata());
                mappedExample.Print(outputStream);
                outputStream << '\n';
                exampleIterator.Next();
            }
        }
    }
    catch (const utilities::CommandLineParserPrintHelpException& exception)
    {
        std::cout << exception.GetHelpText() << std::endl;
        return 0;
    }
    catch (const utilities::CommandLineParserErrorException& exception)
    {
        std::cerr << "Command line parse error:" << std::endl;
        for (const auto& error : exception.GetParseErrors())
        {
            std::cerr << error.GetMessage() << std::endl;
        }
        return 1;
    }
    catch (const utilities::Exception& exception)
    {
        std::cerr << "exception: " << exception.GetMessage() << std::endl;
        return 1;
    }

    return 0;
}
