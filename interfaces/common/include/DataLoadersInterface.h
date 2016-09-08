////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataLoadersInterface.h (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// Our interface classes
#include "RowDatasetInterface.h"

#include "AnyIterator.h"
#include "DataLoadArguments.h"
#include "Example.h"

namespace interfaces
{
    /// <summary> Gets a data iterator from a data file, based on data load arguments. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    ///
    /// <returns> The data iterator. </returns>
    utilities::AnyIterator<dataset::GenericSupervisedExample> GetDataIterator(const std::string& dataFilename);

    /// <summary> Gets a row dataset from data filename. </summary>
    ///
    /// <param name="dataFilename"> The data filename. </param>
    ///
    /// <returns> The row dataset. </returns>
    interfaces::GenericRowDataset GetDataset(const std::string& dataFilename);
}
