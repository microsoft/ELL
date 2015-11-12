// SvmlightParser.tcc

#include "SvmlightParser.h"

#include <sstream>
using std::stringstream;

#include <stdexcept>
using std::runtime_error;

using std::fill;

#include <cctype>
using std::isspace;

namespace dataset
{
    /// Constructs an svmlight data file parser with given options
    ///
    SvmlightParser::SvmlightParser(bool has_weight) : _has_weight(has_weight)
    {}

    /// Parses a Row from svmlight formet
    ///
    SupervisedExample SvmlightParser::Parse(const string& str) 
    {
        // setup stream
        stringstream ss(str);
        double x;
        
        // read first real number
        ss >> x;

        // parse weight
        double weight = 1.0;
        if (_has_weight)
        {
            if (ss.bad()) throw runtime_error("bad stream error while parsing: " + str); // physical problem with the stream
            if (ss.fail()) throw runtime_error("parse error, expected real weight in first token of: " + str); // parse error
            if(ss.eof()) throw runtime_error("empty string: " + str);
            weight = x;

            // read the label
            ss >> x;
        }
        
        // parse label
        if (ss.bad()) throw runtime_error("bad stream error while parsing: " + str); // physical problem with the stream
        if (ss.fail() || ss.eof()) throw runtime_error("parse error, expected real label in: " + str); // parse error
        double label = x;

        uint index;
        double value;
        char ch;

        ss >> index;

        // parse feature values
        while (!ss.eof())
        {
            ss.get(ch);
            ss >> value;
            if (ss.bad()) throw runtime_error("bad stream error while parsing: " + str); // physical problem with the stream
            if (ss.fail() || ch != ':') throw runtime_error("parse error, expected <unsigned int>:<double> format in: " + str); // incorrect format
                                                                                                       
            // store the new value
            _builder.PushBack(index, value);        

            ss >> index;
        }

        return SupervisedExample(_builder.GetVectorAndReset(), label, weight);
    }

    /// Serializes an example to an output stream in svmlight format
    ///
    void SvmlightParser::Print(const SupervisedExample& example, ostream& os) const 
    {
        if(_has_weight)    os << example.GetWeight() << '\t';
        os << example.GetLabel() << '\t';
        example.Print(os);
    }
}