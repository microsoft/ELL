////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     model_python_post.i (interfaces)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%pythoncode %{

# Python friendly class for PortType
class PortType:
    bigInt = PortType_bigInt
    boolean = PortType_boolean
    categorical = PortType_categorical
    integer = PortType_integer
    none = PortType_none
    real = PortType_real
    smallReal = PortType_smallReal
    
# Remove flat enum defines so callers only see the class above
del PortType_bigInt
del PortType_boolean
del PortType_categorical
del PortType_integer
del PortType_none
del PortType_real
del PortType_smallReal

%}