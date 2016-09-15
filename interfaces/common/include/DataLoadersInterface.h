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
#include "RowDataset.h"

namespace emll
{
namespace interfaces
{    
    /// <summary> Gets a row dataset from data filename. </summary>
    ///
    /// <param name="dataFilename"> The data filename. </param>
    ///
    /// <returns> The row dataset. </returns>
//    interfaces::GenericRowDataset2 GetDataset(const std::string& dataFilename);

    emll::dataset::GenericRowDataset GetDataset(const std::string& dataFilename);
}
}
