// IPrintable.h

#pragma once

#include "ElementXLayout.h"
#include "CommandLineArgs.h"

#include "layers.h"
using namespace layers;

#include "types.h"

#include <iostream>
using std::ostream;

#include <memory>
using std::shared_ptr;

struct Point
{
    double x;
    double y;
};

/// An interface for classes that can print human-friendly descriptions of themselves
class IPrintable 
{
public:

    ///// Returns the number of elements in the layer
    /////
    //virtual uint64 Size() const = 0;

    /// Prints a human-firiendly description of the underlying class to an output stream
    ///
    virtual void Print(ostream& os) const = 0;

    /// Computes the layer layout
    ///
    virtual void ComputeLayout(const CommandLineArgs& args, double yOffset) = 0;

    /// \returns the begin-point of an arrow
    ///
    virtual Point GetBeginPoint(uint64 index) const = 0;

    /// \returns the end-point of an arrow
    ///
    virtual Point GetEndPoint(uint64 index) const = 0;

    /// \returns the layer width
    ///
    virtual double GetWidth() const = 0;

    /// \returns the layer height
    ///
    virtual double GetHeight() const = 0;

    /// \returns True if the specified element is visible
    ///
    virtual bool IsHidden(uint64 index) const = 0;

    /// \returns the layer type name
    ///
    virtual string GetTypeName() const = 0;
};

 