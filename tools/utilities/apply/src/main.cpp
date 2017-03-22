////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (apply)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ApplyArguments.h"

// utilities
#include "CommandLineParser.h"
#include "Exception.h"
#include "Files.h"
#include "OutputStreamImpostor.h"

// data
#include "Dataset.h"
#include "Example.h"

// common
#include "DataLoadArguments.h"
#include "DataLoaders.h"
#include "DataSaveArguments.h"
#include "LoadModel.h"
#include "MapLoadArguments.h"
#include "MapSaveArguments.h"

// model
#include "DynamicMap.h"
#include "InputNode.h"
#include "Model.h"
#include "OutputNode.h"

// stl
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>

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
        auto dataIterator = GetExampleIterator(dataLoadArguments);

        // get output stream
        auto& outputStream = dataSaveArguments.outputDataStream;

        // output summarization mode
        if (applyArguments.summarize)
        {
            std::unique_ptr<model::DynamicMap> map2 = nullptr;
            if (applyArguments.inputMapFilename2 != "")
            {
                map2 = std::make_unique<model::DynamicMap>(common::LoadMap(applyArguments.inputMapFilename2));
            }

            math::RowVector<double> u(map.GetOutputSize());
            math::RowVector<double> v(map.GetOutputSize());
            size_t count = 0;

            while (dataIterator.IsValid())
            {
                auto example = dataIterator.Get();
                auto mappedDataVector = map.Compute<data::DoubleDataVector>(example.GetDataVector());
                math::RowVector<double> w(map.GetOutputSize());
                mappedDataVector.AddTo(w);

                if (map2 != nullptr)
                {
                    auto mappedDataVector2 = map2->Compute<data::DoubleDataVector>(example.GetDataVector());
                    mappedDataVector2.AddTo(w, -1.0);
                }

                // accumulate vectors for mean and standard deviation computation
                u += w;
                w.CoordinatewiseSquare();
                v += w;

                dataIterator.Next();
                ++count;
            }

            // calculate and print mean and standard deviation
            double denominator = static_cast<double>(count);
            u /= denominator;
            outputStream << "mean:\t" << u << '\n';

            u.CoordinatewiseSquare();
            v /= denominator;
            v -= u;
            v.CoordinatewiseSquareRoot();
            outputStream << "std:\t" << v << '\n';
        }

        // output new dataset mode
        else
        {
            while (dataIterator.IsValid())
            {
                auto example = dataIterator.Get();
                auto mappedDataVector = map.Compute<data::FloatDataVector>(example.GetDataVector());
                auto mappedExample = data::DenseSupervisedExample(std::move(mappedDataVector), example.GetMetadata());
                mappedExample.Print(outputStream);
                outputStream << '\n';
                dataIterator.Next();
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
