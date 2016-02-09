////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     LinearOperation.tcc (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename PrintableInputType>
void LinearOperation::Print(std::ostream& os, const PrintableInputType& printableInput) const
{
    os << _a << " * (";
    os << printableInput;
    os << ") + " << _b;
}
