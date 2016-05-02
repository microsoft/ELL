////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataLoaders.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "MapLoadArguments.h"
#include "DataLoadArguments.h" 

// dataset
#include "RowDataset.h"

// layers
#include "Map.h"
#include "Coordinate.h"

// dataset
#include "ParsingIterator.h"

// stl
#include <string>


namespace common
{
    /// <summary> Gets a data iterator from a data file, based on data load arguments. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    ///
    /// <returns> The data iterator. </returns>
    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments);

    /// <summary>
    /// Gets a mapped data iterator from a data file, based on command line parameters.
    /// </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    /// <param name="mapLoadArguments"> The map load arguments. </param>
    ///
    /// <returns> The data iterator. </returns>
    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments);

    /// <summary> Gets a row dataset from data load arguments and a map. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    /// <param name="map"> The map. </param>
    ///
    /// <returns> The row dataset. </returns>
    dataset::RowDataset GetRowDataset(const DataLoadArguments& dataLoadArguments, layers::Map map);
}

