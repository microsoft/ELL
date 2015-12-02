// PrintableScale.h

#pragma once

#include "IPrintable.h"
#include "LayerLayout.h"

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
    virtual void ComputeLayout(double xOffset, double yOffset, double xMax) override;

    /// Returns the begin-point of an arrow
    ///
    virtual Point GetBeginPoint(uint64 index) const override;

    /// Returns the end-point of an arrow
    ///
    virtual Point GetEndPoint(uint64 index) const override;

private:
    unique_ptr<LayerLayout> _upLayout = nullptr;
};