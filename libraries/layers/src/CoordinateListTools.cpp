////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CoordinateListTools.cpp (common)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CoordinateListTools.h"
#include "Stack.h"

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
    uint64 ParseIndex(const char*& pStr, uint64 maxIndex)
    {
        uint64 index;
        if (*pStr == 'e')
        {
            index = maxIndex;
            ++pStr;

            if (*pStr == '-')
            {
                ++pStr;
                uint64 diff;
                HandleErrors(utilities::Parse<uint64>(pStr, diff));
                index -= diff;
            }
        }
        else
        {
            HandleErrors(utilities::Parse<uint64>(pStr, index));
        }

        if (index > maxIndex)
        {
            throw std::runtime_error("in coordinate list definition string, index " + std::to_string(index) + " exceeds maximal value " + std::to_string(maxIndex));
        }

        return index;
    }

    // adds an sequence of entries to a coordinateList
    void AddCoordinates(layers::CoordinateList& coordinateList, uint64 layerIndex, uint64 fromElementIndex, uint64 toElementIndex)
    {
        for (uint64 elementIndex = fromElementIndex; elementIndex <= toElementIndex; ++elementIndex)
        {
            coordinateList.emplace_back(layerIndex, elementIndex);
        }
    }
}

namespace layers
{
    layers::CoordinateList GetCoordinateList(const layers::Stack& stack, const std::string& coordinateListString)
    {
        layers::CoordinateList coordinateList;

        // special case for 'e' when map has just 1 layer (the input layer)
        if (coordinateListString == "e" && stack.NumLayers() == 1)
        {
            return coordinateList;
        }

        if(stack.NumLayers() > 0)
        {
            const char* pStr = coordinateListString.c_str();
            const char* pEnd = pStr + coordinateListString.size();


            while(pStr < pEnd)
            {
                // read layer Index
                uint64 layerIndex = ParseIndex(pStr, stack.NumLayers() - 1);
                
                // read element index
                uint64 fromElementIndex = 0;
                uint64 maxElementIndex = stack.GetLayer(layerIndex).Size() - 1; // Fails when layer has size 0
                uint64 toElementIndex = maxElementIndex;
                
                // case: no elements specified - take entire layer
                if(*pStr == ';')
                {
                    ++pStr;
                }
                
                // case: elements specified
                else if(*pStr == ',')
                {
                    ++pStr;
                    fromElementIndex = toElementIndex = ParseIndex(pStr, maxElementIndex);

                    // interval of elements
                    if(*pStr == ':')
                    {
                        ++pStr;
                        toElementIndex = ParseIndex(pStr, maxElementIndex);

                        if(toElementIndex < fromElementIndex)
                        {
                            throw std::runtime_error("bad format in coordinate list definition string");
                        }
                    }
                }

                // add the coordinates to the list
                AddCoordinates(coordinateList, layerIndex, fromElementIndex, toElementIndex);
            }
        }
        
        return coordinateList;
    }
    
    layers::CoordinateList GetCoordinateList(const layers::Stack& stack, uint64 layerIndex)
    {
        return GetCoordinateList(layerIndex, 0, stack.GetLayer(layerIndex).Size()-1);
    }

    layers::CoordinateList GetCoordinateList(uint64 layerIndex, uint64 fromElementIndex, uint64 toElementIndex)
    {
        layers::CoordinateList coordinateList;
        AddCoordinates(coordinateList, layerIndex, fromElementIndex, toElementIndex);
        return coordinateList;
    }
}
