// SvmlightParser.h

#pragma once

#include "SupervisedExample.h"

#include "DatavectorBuilder.h"
using linear::DatavectorBuilder;

#include <string>
using std::string;

#include <iostream>
using std::ostream;

namespace dataset
{
    /// This class implements a parser for the generalized svmlight data file format
    /// Generalized svmlight data file format is defined as follows:
    ///
    /// <line> . = . <weight?> <target> <feature>:<value> ... <feature>:<value> # <comment>
    /// <weight> . = . (double)
    ///    <target> . = . (double)
    ///    <feature> . = . (unsigned integer)
    ///    <value> . = . (double)
    ///    <comment> . = . (string)
    ///
    /// specifying has_weight=false in the ctor modifies the above by "<weight> = ()"
    ///
    /// Note that this format differs from the original svmlight format: (1) we optionally support a weight per example, (2) we do not support the special "qid" feature
    ///
    class SvmlightParser
    {
    public:

        /// Constructs an svmlight data file parser with given options
        ///
        SvmlightParser(bool has_weight=false);

        /// Parses a Row from svmlight formet
        ///
        SupervisedExample Parse(const string& str);

        /// Serializes an example to an output stream in svmlight format
        ///
        void Print(const SupervisedExample& example, ostream& os) const;
    
    private:
        bool _has_weight;
        DatavectorBuilder _builder;
    };
}

