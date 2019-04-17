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


# Python friendly class for UnaryOperationType
class UnaryOperationType:
    none = UnaryOperationType_none
    abs =  UnaryOperationType_abs
    cos = UnaryOperationType_cos
    exp = UnaryOperationType_exp
    hardSigmoid = UnaryOperationType_hardSigmoid
    log = UnaryOperationType_log
    logicalNot = UnaryOperationType_logicalNot
    sigmoid = UnaryOperationType_sigmoid
    sign = UnaryOperationType_sign
    sin = UnaryOperationType_sin
    sqrt = UnaryOperationType_sqrt
    square = UnaryOperationType_square
    softmax = UnaryOperationType_softmax
    tanh = UnaryOperationType_tanh

# Remove flat defines so callers only see the class above
del UnaryOperationType_none
del UnaryOperationType_abs
del UnaryOperationType_cos
del UnaryOperationType_exp
del UnaryOperationType_hardSigmoid
del UnaryOperationType_log
del UnaryOperationType_logicalNot
del UnaryOperationType_sigmoid
del UnaryOperationType_sign
del UnaryOperationType_sin
del UnaryOperationType_sqrt
del UnaryOperationType_square
del UnaryOperationType_softmax
del UnaryOperationType_tanh

# Python friendly class for BinaryOperationType enum
class BinaryOperationType:
    none = BinaryOperationType_none
    add = BinaryOperationType_add
    subtract = BinaryOperationType_subtract
    multiply = BinaryOperationType_multiply
    divide = BinaryOperationType_divide
    logicalAnd = BinaryOperationType_logicalAnd
    logicalOr = BinaryOperationType_logicalOr
    logicalXor = BinaryOperationType_logicalXor

del BinaryOperationType_none
del BinaryOperationType_add
del BinaryOperationType_subtract
del BinaryOperationType_multiply
del BinaryOperationType_divide
del BinaryOperationType_logicalAnd
del BinaryOperationType_logicalOr
del BinaryOperationType_logicalXor
    
import numpy as np

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
    if self.HasSourceNodes():
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

    else:
        import ell
        # legacy direct compute path
        if dtype is np.float:
            return self.ComputeDouble(ell.math.DoubleVector(np.asarray(inputData).astype(dtype)))
        elif dtype is np.float32:
            return self.ComputeFloat(ell.math.FloatVector(np.asarray(inputData).astype(dtype)))
        else:
            raise TypeError("Invalid type, expected numpy.float or numpy.float32")


        

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
    
    vec = np.asarray(inputData).astype(dtype)

    def input_callback(input):
        nonlocal dtype, inputData
        input.copy_from(vec)
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
        if self.HasSourceNodes():
            self.SetSourceCallbackDouble(input_callback, 0)
            self.SetSinkCallbackDouble(results_callback, 0)
            self.StepDouble()
        else:
            # model has no Source/Sink nodes, so use the compute method
            results = self.ComputeDouble(DoubleVector(vec))

    elif dtype is np.float32:
        if self.HasSourceNodes():
            self.SetSourceCallbackFloat(input_callback, 0)
            self.SetSinkCallbackFloat(results_callback, 0)
            self.StepFloat()
        else:
            # model has no Source/Sink nodes, so use the compute method            
            results = self.ComputeFloat(FloatVector(vec))

    else:
        raise TypeError("Invalid type, expected numpy.float or numpy.float32")

    return results

Map.Compute = Map_Compute

# Map.Compile, parameterized on numpy.dtype
def Map_Compile(self, targetDevice: 'std::string const &', moduleName: 'std::string const &', functionName: 'std::string const &', dtype: 'numpy.dtype', compilerOptions: 'MapCompilerOptions const &' = None, optimizerSettings: 'ModelOptimizerOptions const &' = None) -> "ELL_API::CompiledMap< ElementType >":
    """
    Compile(Map self, std::string const & targetDevice, std::string const & moduleName, std::string const & functionName, np.dtype dtype, MapCompilerOptions const & compilerSettings, ModelOptimizerOptions const & optimizerSettings) -> CompiledMap<ElementType>

    Parameters
    ----------
    targetDevice: std::string const &
    moduleName: std::string const &
    functionName: std::string const &
    dtype: numpy.dtype
    compilerOptions: MapCompilerOptions const &
    optimizerSettings: ModelOptimizerOptions const &
    """

    if compilerOptions is None:
        compilerOptions = MapCompilerOptions()

    if optimizerSettings is None:
        optimizerSettings = ModelOptimizerOptions()

    if dtype is np.float:
        return self.CompileDouble(targetDevice, moduleName, functionName, compilerOptions, optimizerSettings)
    elif dtype is np.float32:
        return self.CompileFloat(targetDevice, moduleName, functionName, compilerOptions, optimizerSettings)
    else:
        raise TypeError("Invalid type, expected numpy.float or numpy.float32")

    return None

Map.Compile = Map_Compile

del CompiledMap_Compute
del Map_Compile
del Map_Compute

%}
