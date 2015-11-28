// print.h

#pragma once

#include "IPrintable.h"

/// A class that adds printing capabilities to a layer
///
class PrintableZero : public Zero, public IPrintable
{
    /// Prints a human-friendly description of the layer to a stream
    ///
    virtual void Print(ostream& os, int indentation, int enumeration) override;
};

/// A class that adds printing capabilities to a layer
///
class PrintableScale : public Scale, public IPrintable
{
    /// Prints a human-friendly description of the layer to a stream
    ///
    virtual void Print(ostream& os, int indentation, int enumeration) override;
};

/// A class that adds printing capabilities to a layer
///
class PrintableShift : public Shift, public IPrintable
{
    /// Prints a human-friendly description of the layer to a stream
    ///
    virtual void Print(ostream& os, int indentation, int enumeration) override;
};

/// A class that adds printing capabilities to a layer
///
class PrintableSum : public Sum, public IPrintable
{
    /// Prints a human-friendly description of the layer to a stream
    ///
    virtual void Print(ostream& os, int indentation, int enumeration) override;
};
