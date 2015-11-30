// print.h

#pragma once

#include "IPrintable.h"
#include "layers.h"
using namespace layers;

struct PrintableMap
{
    /// Prints a human-friendly description of the layer to a stream
    ///
    void Print(ostream& os);

    /// Deserializes the Map in json format
    ///
    void Deserialize(JsonSerializer& serializer);

    /// Static function for deserializing shared_ptr<Layer>
    ///
    static void DeserializeLayers(JsonSerializer& serializer, shared_ptr<IPrintable>& up);

private:
    vector<shared_ptr<IPrintable>> _printables;
};

/// A struct that adds printing capabilities to a layer
///
struct PrintableZero : public Zero, public IPrintable
{
    /// Prints a human-friendly description of the layer to a stream
    ///
    virtual void Print(ostream& os) override;
};

/// A struct that adds printing capabilities to a layer
///
struct PrintableScale : public Scale, public IPrintable
{
    /// Prints a human-friendly description of the layer to a stream
    ///
    virtual void Print(ostream& os) override;
};

/// A struct that adds printing capabilities to a layer
///
struct PrintableShift : public Shift, public IPrintable
{
    /// Prints a human-friendly description of the layer to a stream
    ///
    virtual void Print(ostream& os) override;
};

/// A struct that adds printing capabilities to a layer
///
struct PrintableSum : public Sum, public IPrintable
{
    /// Prints a human-friendly description of the layer to a stream
    ///
    virtual void Print(ostream& os) override;
};
