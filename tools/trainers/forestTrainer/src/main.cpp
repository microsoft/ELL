////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (forestTrainer)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <utilities/include/CommandLineParser.h>
#include <utilities/include/Exception.h>
#include <utilities/include/Files.h>
#include <utilities/include/OutputStreamImpostor.h>

#include <data/include/Dataset.h>

#include <common/include/AppendNodeToModel.h>
#include <common/include/DataLoadArguments.h>
#include <common/include/DataLoaders.h>
#include <common/include/EvaluatorArguments.h>
#include <common/include/ForestTrainerArguments.h>
#include <common/include/LoadModel.h>
#include <common/include/MakeEvaluator.h>
#include <common/include/MakeTrainer.h>
#include <common/include/MapLoadArguments.h>
#include <common/include/ModelSaveArguments.h>
#include <common/include/TrainerArguments.h>

#include <model/include/Model.h>

#include <nodes/include/ForestPredictorNode.h>

#include <iostream>
#include <stdexcept>

using namespace ell;

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        common::ParsedTrainerArguments trainerArguments;
        common::ParsedDataLoadArguments dataLoadArguments;
        common::ParsedMapLoadArguments mapLoadArguments;
        common::ParsedModelSaveArguments modelSaveArguments;
        common::ParsedForestTrainerArguments forestTrainerArguments;
        common::ParsedEvaluatorArguments evaluatorArguments;

        commandLineParser.AddOptionSet(trainerArguments);
        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(modelSaveArguments);
        commandLineParser.AddOptionSet(forestTrainerArguments);
        commandLineParser.AddOptionSet(evaluatorArguments);

        // parse command line
        commandLineParser.Parse();

        if (trainerArguments.verbose)
        {
            std::cout << "Sorting Tree Trainer" << std::endl;
            std::cout << commandLineParser.GetCurrentValuesString() << std::endl;
        }

        // load map
        size_t numColumns = dataLoadArguments.parsedDataDimension;
        mapLoadArguments.defaultInputSize = numColumns;

        auto map = common::LoadMap(mapLoadArguments);

        // load dataset
        if (trainerArguments.verbose) std::cout << "Loading data ..." << std::endl;
        auto stream = utilities::OpenIfstream(dataLoadArguments.inputDataFilename);
        auto parsedDataset = common::GetDataset(stream);
        auto mappedDataset = common::TransformDataset(parsedDataset, map);

        // predictor type
        using PredictorType = predictors::SimpleForestPredictor;

        // create trainer and evaluator
        auto trainer = common::MakeForestTrainer(trainerArguments.lossFunctionArguments, forestTrainerArguments);
        auto evaluator = common::MakeEvaluator<PredictorType>(mappedDataset.GetAnyDataset(), evaluatorArguments, trainerArguments.lossFunctionArguments);

        // train
        if (trainerArguments.verbose) std::cout << "Training ..." << std::endl;
        trainer->SetDataset(mappedDataset.GetAnyDataset());

        for (size_t epoch = 0; epoch < trainerArguments.numEpochs; ++epoch)
        {
            trainer->Update();
            evaluator->Evaluate(trainer->GetPredictor());
        }

        auto predictor = trainer->GetPredictor();
        // print loss and errors
        if (trainerArguments.verbose)
        {
            std::cout << "Finished training forest with " << predictor.NumTrees() << " trees." << std::endl;

            // print evaluation
            std::cout << "Training error\n";
            evaluator->Print(std::cout);
            std::cout << std::endl;
        }

        // Save predictor model
        if (modelSaveArguments.outputModelFilename != "")
        {
            auto model = common::AppendNodeToModel<nodes::SimpleForestPredictorNode, PredictorType>(map, predictor);
            common::SaveModel(model, modelSaveArguments.outputModelFilename);
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
