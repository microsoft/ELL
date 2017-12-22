////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     model_python_post.i (interfaces)
//  Authors:  Chris Lovett, Lisa Ong
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

# Compute wrappers for always available (aka. synchronous) inputs
# CompiledMap.Compute, parameterized on numpy.dtype
def CompiledMap_Compute(self, inputData: 'Vector', dtype: 'numpy.dtype') -> "std::vector< double,std::allocator< double > >":
    """
    CompiledMap_Compute(CompiledMap self, std::vector<ElementType> inputData, numpy.dtype dtype) -> std::vector<ElementType>

    Parameters
    ----------
    inputData: std::vector< ElementType,std::allocator< ElementType > > const &
    dtype: numpy.dtype

    """

    def input_callback(input):
        nonlocal dtype, inputData
        input.copy_from(np.asarray(inputData).astype(dtype))
        return True

    results = None
    def results_callback(output):
        nonlocal dtype, results
        if dtype is np.float:
            results = DoubleVector(output)
        else:
            # type checking is already done before we get here
            results = FloatVector(output)


    if dtype is np.float:
        self.RegisterCallbacksDouble(input_callback, results_callback)
        self.StepDouble()
        self.UnregisterCallbacksDouble()
    elif dtype is np.float32:
        self.RegisterCallbacksFloat(input_callback, results_callback)
        self.StepFloat()
        self.UnregisterCallbacksFloat()
    else:
        raise TypeError("Invalid type, expected numpy.float or numpy.float32")

    return results

CompiledMap.Compute = CompiledMap_Compute

# Map.Compute, parameterized on numpy.dtype
def Map_Compute(self, inputData: 'Vector<ElementType>', dtype: 'numpy.dtype') -> "std::vector< ElementType,std::allocator< ElementType > >":
    """
    Compute(Map self, std::vector<ElementType> inputData, numpy.dtype dtype) -> std::vector<ElementType>

    Parameters
    ----------
    inputData: std::vector< ElementType,std::allocator< ElementType > > const &
    dtype: numpy.dtype

    """
    def input_callback(input):
        nonlocal dtype, inputData
        input.copy_from(np.asarray(inputData).astype(dtype))
        return True

    results = None
    def results_callback(output):
        nonlocal dtype, results
        if dtype is np.float:
            results = DoubleVector(output)
        else:
            # type checking is already done before we get here
            results = FloatVector(output)

    if dtype is np.float:
        self.SetSourceCallbackDouble(input_callback, 0)
        self.SetSinkCallbackDouble(results_callback, 0)
        self.StepDouble()
    elif dtype is np.float32:
        self.SetSourceCallbackFloat(input_callback, 0)
        self.SetSinkCallbackFloat(results_callback, 0)
        self.StepFloat()
    else:
        raise TypeError("Invalid type, expected numpy.float or numpy.float32")

    return results

Map.Compute = Map_Compute

# Map.Compile, parameterized on numpy.dtype
def Map_Compile(self, targetDevice: 'std::string const &', moduleName: 'std::string const &', functionName: 'std::string const &', useBlas: 'bool', dtype: 'numpy.dtype') -> "ELL_API::CompiledMap< ElementType >":
    """
    Compile(Map self, std::string const & targetDevice, std::string const & moduleName, std::string const & functionName, bool useBlas, np.dtype dtype) -> CompiledMap<ElementType>

    Parameters
    ----------
    targetDevice: std::string const &
    moduleName: std::string const &
    functionName: std::string const &
    useBlas: bool
    dtype: numpy.dtype

    """

    if dtype is np.float:
        return self.CompileDouble(targetDevice, moduleName, functionName, useBlas)
    elif dtype is np.float32:
        return self.CompileFloat(targetDevice, moduleName, functionName, useBlas)
    else:
        raise TypeError("Invalid type, expected numpy.float or numpy.float32")

    return None

Map.Compile = Map_Compile

%}