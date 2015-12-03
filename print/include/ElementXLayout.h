// ElementXLayout.h

# pragma once

#include "CommandLineArgs.h"

#include "types.h"

#include "Layer.h"
using layers::Layer;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <stdexcept>
using std::runtime_error;

class ElementXLayout
{
public:

    /// Ctor
    ///
    ElementXLayout(uint64 numElements, const CommandLineArgs& args);

    /// \returns the x-axis value of the middle of the requested element
    ///
    double GetXMid(uint64 index) const;

    /// \returns the layer width
    ///
    double GetWidth() const;

    /// \returns true if the layer is presented in abbreviated form
    ///
    bool IsAbbreviated() const;

    /// \returns the number of elements before the dots
    ///
    uint64 GetNumElementsBeforeDots() const;

    /// \returns the x-axis value of the middle of the dots
    ///
    double GetDotsXMid() const;

private:
    vector<double> _elementXMid;
    bool _abbreviated = false;
    uint64 _numElementsBeforeDots = 0;
    double _layerWidth = 0;
    double _dotsXMid = 0;
};