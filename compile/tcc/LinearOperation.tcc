// LinearOperation.tcc

template<typename PrintableInputType>
void LinearOperation::Print(ostream& os, const PrintableInputType& printableInput) const
{
    os << _a << " * (";
    os << printableInput;
    os << ") + " << _b;
}
