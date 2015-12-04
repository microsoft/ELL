// PrintableCoordinatewise.h

#pragma once

#include "IPrintable.h"
#include "ElementXLayout.h"

#include "layers.h"
using namespace layers;

#include <memory>
using std::unique_ptr;

/// A struct that adds printing capabilities to a layer
///
struct PrintableCoordinatewise : public Scale, public IPrintable
{
public:
    /// Ctor
    ///
    PrintableCoordinatewise(string typeName, string svgClass);    

    /// Prints a human-firiendly description of the underlying class to an output stream
    ///
    virtual void Print(ostream& os, uint64 index, const vector<shared_ptr<IPrintable>>& layers) const override;

    /// Computes the layer layout
    ///
    virtual void ComputeLayout(const CommandLineArgs& args, double yOffset) override;

    /// Returns the begin-point of an arrow
    ///
    virtual Point GetBeginPoint(uint64 index) const override;

    /// Returns the end-point of an arrow
    ///
    virtual Point GetEndPoint(uint64 index) const override;

    // \returns the layer width
    //
    virtual double GetWidth() const;

    // \returns the layer height
    //
    virtual double GetHeight() const;

private:
    unique_ptr<ElementXLayout> _upLayout = nullptr;

    string _typeName;
    string _svgClass;

    double _x;
    double _cy;
    double _layerHeight;
    double _layerCornerRadius;
    double _elementWidth;
    double _elementHeight;
    double _elementCornerRadius;
    double _elementConnectorRadius;
    int _valueMaxChars;
};