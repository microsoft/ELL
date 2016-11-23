////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
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
#include "Example.h"
#include "Dataset.h"

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

using namespace emll;

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
        auto dataIterator = GetDataIterator(dataLoadArguments);

        // get output stream
        auto& outputStream = dataSaveArguments.outputDataStream;

        // apply map to dataset
        if (applyArguments.inputMapFilename2 != "")
        {
            auto map2 = common::LoadMap(applyArguments.inputMapFilename2);
            math::RowVector<double> mean(map.ComputeSize());
            math::RowVector<double> meanSquared(map.ComputeSize());
            size_t count = 0;

            while (dataIterator->IsValid())
            {
                auto example = dataIterator->Get();
                auto mappedDataVector = map.Compute<data::DoubleDataVector>(example.GetDataVector());
                auto mappedDataVector2 = map2.Compute<data::DoubleDataVector>(example.GetDataVector());

                math::RowVector<double> v(map.ComputeSize());
                mappedDataVector.AddTo(v);
                mappedDataVector2.AddTo(v, -1.0);

                math::Operations::Add(1.0, v, mean);
                v.Transform([](double x) {return x*x; });
                math::Operations::Add(1.0, v, meanSquared);

                dataIterator->Next();
                ++count;
            }

            math::Operations::Multiply(1.0 / count, mean);
            meanSquared.Transform([count](double x) {return std::sqrt(x / count); });

            outputStream << "mean:\t";
            mean.Print(outputStream);
            outputStream << '\n';
            outputStream << "std:\t";
            meanSquared.Print(outputStream);
            outputStream << '\n';
        }

        else if (applyArguments.summarize)
        {
            math::RowVector<double> mean(map.ComputeSize());
            math::RowVector<double> meanSquared(map.ComputeSize());
            size_t count = 0;

            while (dataIterator->IsValid())
            {
                auto example = dataIterator->Get();
                auto mappedDataVector = map.Compute<data::FloatDataVector>(example.GetDataVector());
                
                math::RowVector<double> v(map.ComputeSize());
                mappedDataVector.AddTo(v);
                math::Operations::Add(1.0, v, mean);
                v.Transform([](double x) {return x*x; });
                math::Operations::Add(1.0, v, meanSquared);

                dataIterator->Next();
                ++count;
            }

            math::Operations::Multiply(1.0 / count, mean);
            meanSquared.Transform([count](double x) {return std::sqrt(x / count); });

            outputStream << "mean:\t";
            mean.Print(outputStream);
            outputStream << '\n';
            outputStream << "std:\t";
            meanSquared.Print(outputStream);
            outputStream << '\n';
        }
        
        else 
        {
            while (dataIterator->IsValid())
            {
                auto example = dataIterator->Get();
                auto mappedDataVector = map.Compute<data::FloatDataVector>(example.GetDataVector());
                auto mappedExample = data::DenseSupervisedExample(std::move(mappedDataVector), example.GetMetadata());
                mappedExample.Print(outputStream);
                outputStream << '\n';
                dataIterator->Next();
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
