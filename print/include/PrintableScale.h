// PrintableScale.h

#pragma once

#include "IPrintable.h"
#include "ElementXLayout.h"

#include "layers.h"
using namespace layers;

#include <memory>
using std::unique_ptr;

/// A struct that adds printing capabilities to a layer
///
struct PrintableScale : public Scale, public IPrintable
{
public:
    /// Prints a human-firiendly description of the underlying class to an output stream
    ///
    virtual void Print(ostream& os, const vector<shared_ptr<IPrintable>>& layers) const override;

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
    double _xLayerIndent = 0;
    double _yLayerOffset = 0;
    double _elementWidth = 0;
    double _elementHeight = 0;
    double _yElementPadding = 0;

};