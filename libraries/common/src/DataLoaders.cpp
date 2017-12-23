////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataLoaders.cpp (common)
//  Authors:  Ofer Dekel, Chuck Jacobs, Byron Changuion
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

// stl
#include <memory>
#include <stdexcept>

namespace ell
{
namespace common
{

    data::AutoSupervisedExampleIterator GetAutoSupervisedExampleIterator(std::istream& stream)
    {
        return GetExampleIterator<data::SequentialLineIterator, data::LabelParser, data::AutoDataVectorParser<data::GeneralizedSparseParsingIterator>>(stream);
    }

    data::AutoSupervisedMultiClassExampleIterator GetAutoSupervisedMultiClassExampleIterator(std::istream& stream)
    {
        return GetExampleIterator<data::SequentialLineIterator, data::ClassIndexParser, data::AutoDataVectorParser<data::GeneralizedSparseParsingIterator>>(stream);
    }

    data::AutoSupervisedDataset GetDataset(std::istream& stream)
    {
        return data::MakeDataset(GetExampleIterator<data::SequentialLineIterator, data::LabelParser, data::AutoDataVectorParser<data::GeneralizedSparseParsingIterator>>(stream));
    }

    data::AutoSupervisedMultiClassDataset GetMultiClassDataset(std::istream& stream)
    {
        return data::MakeDataset(GetExampleIterator<data::SequentialLineIterator, data::ClassIndexParser, data::AutoDataVectorParser<data::GeneralizedSparseParsingIterator>>(stream));
    }
}
}
