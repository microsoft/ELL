////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     CoordinateListTools.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CoordinateListTools.h"
#include "Model.h"

// utilities
#include "Parser.h"

namespace
{
    void HandleErrors(utilities::ParseResult result)
    {
        if (result == utilities::ParseResult::badFormat)
        {
            throw std::runtime_error("bad format in coordinate list definition string");
        }
        else if (result == utilities::ParseResult::endOfString || result == utilities::ParseResult::beginComment)
        {
            throw std::runtime_error("premature end of coordinate list definition string");
        }
    }

    // Parses the c-string pStr. Allowed values are (1) non-negative integers not greater than maxIndex, (2) "e", which translates to maxIndex, (3) "e-<uint>" which translates into maxIndex minus the integer value, as long as the outcome is in the range [0,maxValue]
    uint64_t ParseIndex(const char*& pStr, uint64_t size)
    {
        uint64_t index;
        if (*pStr == 'e')
        {
            if (size == 0)
            {
                throw std::runtime_error("automatic index 'e' used, but layer size cannot be inferred (for input layer, use -dd <dimension> or -dd auto)");
            }
            index = size-1;
            ++pStr;

            if (*pStr == '-')
            {
                ++pStr;
                uint64_t diff;
                HandleErrors(utilities::Parse<uint64_t>(pStr, diff));
                index -= diff;
            }
        }
        else
        {
            HandleErrors(utilities::Parse<uint64_t>(pStr, index));
        }

        return index;
    }

    // adds an sequence of entries to a coordinateList
    void AddCoordinates(layers::CoordinateList& coordinateList, uint64_t layerIndex, uint64_t fromElementIndex, uint64_t endElementIndex)
    {
        for (uint64_t elementIndex = fromElementIndex; elementIndex < endElementIndex; ++elementIndex)
        {
            coordinateList.AddCoordinate(layerIndex, elementIndex);
        }
    }
}

namespace layers
{
    layers::CoordinateList BuildCoordinateList(const layers::Model& model, uint64_t inputLayerSize, const std::string& coordinateListString)
    {
        layers::CoordinateList coordinateList;

        const char* pStr = coordinateListString.c_str();
        const char* pEnd = pStr + coordinateListString.size();

        while (pStr < pEnd)
        {
            // read layer Index
            uint64_t layerIndex = ParseIndex(pStr, model.NumLayers());

            // read element index
            uint64_t fromElementIndex = 0;
            uint64_t layerSize;
            if (layerIndex == 0)
            {
                layerSize = inputLayerSize;
            }
            else
            {
                layerSize = model.GetLayer(layerIndex).Size();
            }
            uint64_t endElementIndex = layerSize;

            // case: no elements specified - take entire layer
            if (*pStr == ';')
            {
                ++pStr;
            }

            // case: elements specified
            else if (*pStr == ',')
            {
                ++pStr;
                fromElementIndex = ParseIndex(pStr, layerSize);
                endElementIndex = fromElementIndex + 1;

                // interval of elements
                if (*pStr == ':')
                {
                    ++pStr;
                    endElementIndex = ParseIndex(pStr, layerSize)+1;

                    if (endElementIndex <= fromElementIndex)
                    {
                        throw std::runtime_error("bad format in coordinate list definition string");
                    }
                }
            }

            // check that the coordiates are compatible with the model
            if (layerIndex == 0)
            {
                if (endElementIndex == 0)
                {
                    throw std::runtime_error("input layer size cannot be inferred (use - dd <dimension> or -dd auto)");
                }
            }
            else
            {
                if (endElementIndex > layerSize)
                {
                    throw std::runtime_error("coordinate list index exceeds layer size");
                }
            }

            // add the coordinates to the list
            AddCoordinates(coordinateList, layerIndex, fromElementIndex, endElementIndex);
        }

        return coordinateList;
    }
}
