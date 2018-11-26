////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataLoaders.cpp (common)
//  Authors:  Ofer Dekel, Chuck Jacobs, Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataLoaders.h"

#include <utilities/include/Files.h>

#include <data/include/Dataset.h>
#include <data/include/SequentialLineIterator.h>

#include <data/include/AutoDataVector.h>
#include <data/include/GeneralizedSparseParsingIterator.h>
#include <data/include/SingleLineParsingExampleIterator.h>
#include <data/include/WeightLabel.h>

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
} // namespace common
} // namespace ell
