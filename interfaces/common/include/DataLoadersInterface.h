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

#include "DataLoadArguments.h"
#include "SupervisedExample.h"
#include "AnyIterator.h"
\
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
