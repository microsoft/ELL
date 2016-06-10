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
#include "MapInterface.h"
#include "ModelInterface.h"

#include "DataLoadArguments.h"
#include "MapLoadArguments.h"
#include "ModelLoadArguments.h"
#include "SupervisedExample.h"
#include "AnyIterator.h"
#include "Map.h"

namespace interfaces
{
    /// <summary> Gets a data iterator from a data file, based on data load arguments. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    ///
    /// <returns> The data iterator. </returns>
    utilities::AnyIterator<dataset::GenericSupervisedExample> GetDataIterator(const std::string& dataFilename);

    /// <summary>
    /// Gets a mapped data iterator from a data file, based on command line parameters.
    /// </summary>
    ///
    /// <param name="dataFilename"> The data file name. </param>
    /// <param name="dimension"> The number of columns of the dataset. </param>
    /// <param name="coordinateListString"> The coordinate list. </param>
    /// <param name="modelFilename"> The model filename. </param>
    ///
    /// <returns> The data iterator. </returns>
    utilities::AnyIterator<dataset::GenericSupervisedExample> GetDataIterator(const std::string& dataFilename, int dimension, const std::string coordinateListString, const std::string& modelFilename);

    /// <summary> Gets a row dataset from data filename. </summary>
    ///
    /// <param name="dataFilename"> The data filename. </param>
    ///
    /// <returns> The row dataset. </returns>
    interfaces::GenericRowDataset GetDataset(const std::string& dataFilename);

    /// <summary> Gets a row dataset from data filename and a map. </summary>
    ///
    /// <param name="dataFilename"> The data filename. </param>
    /// <param name="map"> The map. </param>
    ///
    /// <returns> The row dataset. </returns>
    interfaces::GenericRowDataset GetMappedDataset(const std::string& dataFilename, const interfaces::Map& map);

    /// <summary> Gets a row dataset from data filename and a map filename. </summary>
    ///
    /// <param name="dataFilename"> The data filename. </param>
    /// <param name="mapFilename"> The map filename. </param>
    ///
    /// <returns> The row dataset. </returns>
    interfaces::GenericRowDataset GetDataset(const std::string& dataFilename, const std::string& mapFilename);
}
