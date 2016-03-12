////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     DataLoaders.cpp (common)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataLoaders.h"
#include "CoordinateListTools.h"

// utilities
#include "Files.h"

// dataset
#include "SequentialLineIterator.h"
#include "SparseEntryParser.h"
#include "MappedParser.h"
#include "ParsingIterator.h"

// stl
#include <memory>
#include <stdexcept>

namespace common
{
    dataset::RowDataset LoadDataset(dataset::IParsingIterator& dataIterator)
    {
        dataset::RowDataset dataset;
        while (dataIterator.IsValid())
        {
            dataset.PushBackRow(dataIterator.Get());
            dataIterator.Next();
        }
        return dataset;
    }

    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments)
    {
        // create line iterator - read line by line sequentially
        dataset::SequentialLineIterator lineIterator(dataLoadArguments.inputDataFile);

        // create parser for sparse vectors (SVMLight format)
        dataset::SparseEntryParser sparseEntryParser;

        // Create iterator
        return dataset::GetParsingIterator(std::move(lineIterator), sparseEntryParser);
    }

    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments)
    {
        if (mapLoadArguments.inputMapFile == "")
        {
            return GetDataIterator(dataLoadArguments);
        }
        else
        {
            auto map = GetMap(mapLoadArguments);
            if(map == nullptr)
            {
                throw std::runtime_error("Error: couldn't load map");
            }
            auto inputCoordinates = GetInputCoordinates(*map, mapLoadArguments);
            return GetMappedDataIterator(dataLoadArguments, map, inputCoordinates);

            /*
            // TODO:
            auto inputIterator = GetDataIterator(dataLoadArguments);
            auto map = GetMap(mapLoadArguments);
            auto inputCoordinates = GetInputCoordinates(*map, mapLoadArguments);
            return GetMappedDataIterator(std::move(inputIterator), map, inputCoordinates);
            */
        }
    }

    std::shared_ptr<layers::Map> GetMap(const MapLoadArguments& mapLoadArguments)
    {
        if (mapLoadArguments.inputMapFile != "")
        {
            return std::make_shared<layers::Map>(layers::Map::Load(mapLoadArguments.inputMapFile));
        }
        else
        {
            // #### TODO: return empty "identity" map
            return nullptr;
        }
    }

    std::unique_ptr<dataset::IParsingIterator> GetMappedDataIterator(const DataLoadArguments& dataLoadArguments, const std::shared_ptr<layers::Map>& map, const layers::CoordinateList& inputCoordinates)
    {
        if(map == nullptr)
        {
            return GetDataIterator(dataLoadArguments);
        }

        // create parser for sparse vectors (SVMLight format)
        dataset::SparseEntryParser sparseEntryParser;

        // create mapped parser
        dataset::MappedParser<dataset::SparseEntryParser> mappedParser(sparseEntryParser, map, inputCoordinates);

        // #### why not just run dataIterator through the map?

        // create line iterator - read line by line sequentially
        dataset::SequentialLineIterator lineIterator(dataLoadArguments.inputDataFile);

        // Create iterator
        return dataset::GetParsingIterator(std::move(lineIterator), mappedParser);
    }


    /*    
    class MappedDataIterator : public dataset::IParsingIterator
    {
    public:
        MappedDataIterator(std::shared_ptr<layers::Map> map, const layers::CoordinateList& inputCoordinates);

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if it succeeds, false if it fails. </returns>
        virtual bool IsValid() const;

        /// <summary> Proceeds to the Next row. </summary>
        virtual void Next();

        /// <summary> \returns The weight of the current example. </summary>
        ///
        /// <returns> A SupervisedExample. </returns>
        virtual dataset::SupervisedExample Get() const;

    private:
        std::shared_ptr<layers::Map> _map;
        layers::CoordinateList _inputCoordinates;
    };

    std::unique_ptr<dataset::IParsingIterator> GetMappedDataIterator(std::unique_ptr<dataset::IParsingIterator>&& inputIterator, const std::shared_ptr<layers::Map>& map, const layers::CoordinateList& inputCoordinates)
    {
        return std::make_unique<MappedDataIterator>(inputIterator, map, inputCoordinates);
    }
*/
    
    layers::CoordinateList GetInputCoordinates(const layers::Map& map, const MapLoadArguments& mapLoadArguments)
    {
        layers::CoordinateList inputCoordinates = layers::GetCoordinateList(map, mapLoadArguments.coordinateList);
        return inputCoordinates;
    }

    void GetRowDatasetMapCoordinates(
        const DataLoadArguments& dataLoadArguments,
        const MapLoadArguments& mapLoadArguments,
        dataset::RowDataset& rowDataset,
        std::shared_ptr<layers::Map>& map,
        layers::CoordinateList& inputCoordinates)
    {
        // handle two cases - input map specified or unspecified
        if (mapLoadArguments.inputMapFile == "")
        {
            // get data iterator 
            auto upDataIterator = GetDataIterator(dataLoadArguments);

            // load dataset
            rowDataset = LoadDataset(*upDataIterator);

            // number of columns
            uint64 numColumns = rowDataset.NumColumns();

            // create default map with single input layer
            map = std::make_shared<layers::Map>(numColumns);

            // create a coordinate list of this map
            inputCoordinates = layers::GetCoordinateList(0, 0, numColumns-1);
        }
        else
        {
            // read map from file
            map = GetMap(mapLoadArguments);

            // read input coordinates
            inputCoordinates = GetInputCoordinates(*map, mapLoadArguments);

            // fill in dataset
            auto upDataIterator = GetMappedDataIterator(dataLoadArguments, map, inputCoordinates);
            rowDataset = LoadDataset(*upDataIterator);
        }
    }
    
    // Do this instead of calling GetRowDatasetMapCoordinates
    void MyCodeInMain(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments)
    {
        auto inputIterator = GetDataIterator(dataLoadArguments);
        auto map = GetMap(mapLoadArguments);
        dataset::RowDataset rowDataset; // depends on map if there is one, else depends on inputIterator
        layers::CoordinateList inputCoordinates; // depends on dataset if there is one, else depends on map

        // need to set rowDataset and inputCoordinates
        if (map == nullptr)
        {
            rowDataset = LoadDataset(*inputIterator);
            auto numColumns = rowDataset.NumColumns();
            map = std::make_unique<layers::Map>(numColumns); // :(
            inputCoordinates = layers::GetCoordinateList(0, 0, numColumns - 1);
        }
        else
        {
            inputCoordinates = GetInputCoordinates(*map, mapLoadArguments);
            auto dataIterator = GetMappedDataIterator(dataLoadArguments, map, inputCoordinates);
            rowDataset = LoadDataset(*dataIterator);
        }
    }
}
