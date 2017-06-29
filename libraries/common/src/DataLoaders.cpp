////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataLoaders.cpp (common)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataLoaders.h"

// utilities
#include "Files.h"

// data
#include "Dataset.h"
#include "SequentialLineIterator.h"

#include "SingleLineParsingExampleIterator.h"
#include "AutoDataVector.h"
#include "WeightLabel.h"
#include "GeneralizedSparseParsingIterator.h"

// model
#include "DynamicMap.h"

// stl
#include <memory>
#include <stdexcept>

namespace ell
{
namespace common
{
    data::AutoSupervisedExampleIterator GetExampleIterator(std::istream& stream)
    {
        data::SequentialLineIterator textLineIterator(stream); 

        data::LabelParser metadataParser;

        data::AutoDataVectorParser<data::GeneralizedSparseParsingIterator> dataVectorParser;

        return data::MakeSingleLineParsingExampleIterator(std::move(textLineIterator), std::move(metadataParser), std::move(dataVectorParser));
    }

    data::AutoSupervisedDataset GetDataset(std::istream& stream)
    {
        return data::MakeDataset(GetExampleIterator(stream));
    }
}
}
