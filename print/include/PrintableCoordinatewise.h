//// PrintableCoordinatewise.h
//
//#pragma once
//
//#include "IPrintableLayer.h"
//#include "LayerLayout.h"
//
//#include "Coordinatewise.h"
//using layers::Coordinatewise;
//
//#include <memory>
//using std::unique_ptr;
//
///// A struct that adds printing capabilities to a layer
/////
//struct PrintableCoordinatewise : public Coordinatewise, public IPrintableLayer
//{
//public:
//    /// Prints a human-firiendly description of the underlying class to an output stream
//    ///
//    virtual LayerLayout Print(ostream& os, double left, double top, const CommandLineArguments& args) const override;
//};