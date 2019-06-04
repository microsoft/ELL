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
def CompiledMap_Compute(self, inputData):
    """
    Compute - takes an input array or typed vector that is input to the model.
    Returns the result as a typed vector (FloatVector, DoubleVector, etc).

    Parameters
    ----------
    inputData: an input numpy array or vector

    """
    import ell
    dtype = self.GetInputType(0)
    if dtype == ell.nodes.PortType.real:
        if isinstance(inputData, DoubleVector):
            return self.ComputeDouble(inputData)
        else:
            return self.ComputeDouble(DoubleVector(np.asarray(inputData).astype(np.float)))
    elif dtype == ell.nodes.PortType.smallReal:
        if isinstance(inputData, FloatVector):
            return self.ComputeFloat(inputData)
        else:
            return self.ComputeFloat(FloatVector(np.asarray(inputData).astype(np.float32)))
    elif dtype == ell.nodes.PortType.integer:
        if isinstance(inputData, IntVector):
            return self.ComputeInt(inputData)
        else:
            return self.ComputeInt(IntVector(np.asarray(inputData).astype(np.int)))
    #elif dtype == ell.nodes.PortType.bigInt:
    #    if isinstance(inputData, Int64Vector):
    #        return self.ComputeInt64(inputData)
    #    else:
    #        return self.ComputeInt64(Int64Vector(np.asarray(inputData).astype(np.int64)))
    else:
        raise TypeError("Cannot Compute model on input type : " + str(dtype))

CompiledMap.Compute = CompiledMap_Compute
del CompiledMap_Compute

# Map.Compute, parameterized on numpy.dtype
def Map_Compute(self, inputData):
    """
    Compute - takes an input array or typed vector that is input to the model.
    Returns the result as a typed vector (FloatVector, DoubleVector, etc).

    Parameters
    ----------
    inputData: an input numpy array or vector
    """
    import ell
    dtype = self.GetInputType(0)
    if dtype == ell.nodes.PortType.real:
        if isinstance(inputData, DoubleVector):
            return self.ComputeDouble(inputData)
        else:
            return self.ComputeDouble(DoubleVector(np.asarray(inputData).astype(np.float)))
    elif dtype == ell.nodes.PortType.smallReal:
        if isinstance(inputData, FloatVector):
            return self.ComputeFloat(inputData)
        else:
            return self.ComputeFloat(FloatVector(np.asarray(inputData).astype(np.float32)))
    elif dtype == ell.nodes.PortType.integer:
        if isinstance(inputData, IntVector):
            return self.ComputeInt(inputData)
        else:
            return self.ComputeInt(IntVector(np.asarray(inputData).astype(np.int)))
    else:
        raise TypeError("Model has unsupported input type : " + str(dtype))

Map.Compute = Map_Compute
del Map_Compute
    
    
# Map.Compile, parameterized on numpy.dtype
def Map_Compile(self, targetDevice: 'std::string const &', moduleName: 'std::string const &', functionName: 'std::string const &', compilerOptions: 'MapCompilerOptions const &' = None, optimizerSettings: 'ModelOptimizerOptions const &' = None) -> "ELL_API::CompiledMap< ElementType >":
    """
    Compile(Map self, std::string const & targetDevice, std::string const & moduleName, std::string const & functionName, MapCompilerOptions const & compilerSettings, ModelOptimizerOptions const & optimizerSettings) -> CompiledMap<ElementType>

    Parameters
    ----------
    targetDevice: std::string const &
    moduleName: std::string const &
    functionName: std::string const &
    compilerOptions: MapCompilerOptions const &
    optimizerSettings: ModelOptimizerOptions const &
    """

    if compilerOptions is None:
        compilerOptions = MapCompilerOptions()

    if optimizerSettings is None:
        optimizerSettings = ModelOptimizerOptions()

    return self.InternalCompile(targetDevice, moduleName, functionName, compilerOptions, optimizerSettings)

Map.InternalCompile = Map.Compile
Map.Compile = Map_Compile
del Map_Compile

def SourceNode_RegisterCallback(self, function):
    """
    Register a callback function.  This function will be called when this SourceNode needs data.
    The registered function receives a typed Vector argument (ell.math.DoubleVector, FloatVector, etc)
    and returns a boolean indicating whether or not it filled the buffer with new data.
    """
    outputPort = self.GetOutputPorts().Get()
    portType = outputPort.GetOutputType()
    import ell

    if portType == ell.nodes.PortType.real:
        class SourceNodeDoubleCallbackBase(ell.math.DoubleCallbackBase):
            def __init__(self, function):
                super(SourceNodeDoubleCallbackBase, self).__init__()
                self.func = function
            def Run(self, data):
                self.func(data)
                return True
        self.wrapper = SourceNodeDoubleCallbackBase(function)  # must stay alive
        self.RegisterCallbackDouble(self.wrapper)
    elif portType == ell.nodes.PortType.smallReal:        
        class SourceNodeFloatCallbackBase(ell.math.FloatCallbackBase):
            def __init__(self, function):
                super(SourceNodeFloatCallbackBase, self).__init__()
                self.func = function
            def Run(self, data):
                self.func(data)
                return True
        self.wrapper = SourceNodeFloatCallbackBase(function)  # must stay alive
        self.RegisterCallbackDouble(self.wrapper)
    elif portType == ell.nodes.PortType.integer:      
        class SourceNodeIntCallbackBase(ell.math.IntCallbackBase):
            def __init__(self, function):
                super(SourceNodeIntCallbackBase, self).__init__()
                self.func = function
            def Run(self, data):
                self.func(data)
                return True
        self.wrapper = SourceNodeIntCallbackBase(function)  # must stay alive
        self.RegisterCallbackDouble(self.wrapper)
    #elif portType == ell.nodes.PortType.bigInt:     
    #    class SourceNodeInt64CallbackBase(ell.math.Int64CallbackBase):
    #        def __init__(self, function):
    #            super(SourceNodeInt64CallbackBase, self).__init__()
    #            self.func = function
    #        def Run(self, data):
    #            self.func(data)
    #            return True
    #    self.wrapper = SourceNodeInt64CallbackBase(function)  # must stay alive
    #    self.RegisterCallbackDouble(self.wrapper)
    elif portType == ell.nodes.PortType.boolean:
        class SourceNodeInt8CallbackBase(ell.math.Int8CallbackBase):
            def __init__(self, function):
                super(SourceNodeInt8CallbackBase, self).__init__()
                self.func = function
            def Run(self, data):
                self.func(data)
                return True
        self.wrapper = SourceNodeInt8CallbackBase(function)  # must stay alive
        self.RegisterCallbackDouble(self.wrapper)
    else:
        raise Exception("portType {} not supported".format(portType))

SourceNode.RegisterCallback = SourceNode_RegisterCallback
del SourceNode_RegisterCallback

def SinkNode_RegisterCallback(self, function):
    """
    Register a callback function.  This function will be called when this SinkNode has data to provide.
    The registered function receives a typed Vector argument (ell.math.DoubleVector, FloatVector, etc)
    containing the data.
    """
    outputPort = self.GetOutputPorts().Get()
    portType = outputPort.GetOutputType()
    import ell
    if portType == ell.nodes.PortType.real:
        class SinkNodeDoubleCallbackBase(ell.math.DoubleCallbackBase):
            def __init__(self, function):
                super(SinkNodeDoubleCallbackBase, self).__init__()
                self.func = function
            def Run(self, data):
                self.func(data)
                return True
        self.wrapper = SinkNodeDoubleCallbackBase(function)  # must stay alive
        self.RegisterCallbackDouble(self.wrapper)
    elif portType == ell.nodes.PortType.smallReal:        
        class SinkNodeFloatCallbackBase(ell.math.FloatCallbackBase):
            def __init__(self, function):
                super(SinkNodeFloatCallbackBase, self).__init__()
                self.func = function
            def Run(self, data):
                self.func(data)
                return True
        self.wrapper = SinkNodeFloatCallbackBase(function)  # must stay alive
        self.RegisterCallbackDouble(self.wrapper)
    elif portType == ell.nodes.PortType.integer:      
        class SinkNodeIntCallbackBase(ell.math.IntCallbackBase):
            def __init__(self, function):
                super(SinkNodeIntCallbackBase, self).__init__()
                self.func = function
            def Run(self, data):
                self.func(data)
                return True
        self.wrapper = SinkNodeIntCallbackBase(function)  # must stay alive
        self.RegisterCallbackDouble(self.wrapper)
    #elif portType == ell.nodes.PortType.bigInt:     
    #    class SinkNodeInt64CallbackBase(ell.math.Int64CallbackBase):
    #        def __init__(self, function):
    #            super(SinkNodeInt64CallbackBase, self).__init__()
    #            self.func = function
    #        def Run(self, data):
    #            self.func(data)
    #            return True
    #    self.wrapper = SinkNodeInt64CallbackBase(function)  # must stay alive
    #    self.RegisterCallbackDouble(self.wrapper)
    elif portType == ell.nodes.PortType.boolean:
        class SinkNodeInt8CallbackBase(ell.math.Int8CallbackBase):
            def __init__(self, function):
                super(SinkNodeInt8CallbackBase, self).__init__()
                self.func = function
            def Run(self, data):
                self.func(data)
                return True
        self.wrapper = SinkNodeInt8CallbackBase(function)  # must stay alive
        self.RegisterCallbackDouble(self.wrapper)
    else:
        raise Exception("portType {} not supported".format(portType))

SinkNode.RegisterCallback = SinkNode_RegisterCallback
del SinkNode_RegisterCallback

%}

%{
#include <utilities/include/TypeName.h>

template<typename ElementType>
void ExtractBufferFromPythonList(std::shared_ptr<ell::model::Map> map, PyObject* list, size_t i, std::vector<void*>& args)
{
    auto item = PyList_GetItem(list, i);
    std::vector<ElementType>* ptr = nullptr;
    auto res = swig::asptr(item, &ptr);
    if (!SWIG_IsOK(res)) 
    {                        
        std::string typeName = ell::utilities::TypeName<ElementType>::GetName();
        throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong type, expecting '%s*'", i, typeName.c_str()));
    }
    auto argSize = map->GetInputSize(i);
    if (argSize != ptr->size())
    {
        throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong size, expecting '%zu'", i, argSize));
    }
    args.push_back(ptr->data());
    if (SWIG_IsNewObj(res)) 
    {
        throw std::invalid_argument("unexpected new object, this will leak!");
    }            
}

std::vector<void*> GetInputBuffersFromList(std::shared_ptr<ell::model::Map> map, PyObject *list) 
{
    // Here we expect a list of arguments, each one is a pre-allocated FloatVector or DoubleVector or AutoDataVector
    // for all the expected inputs of the model.
    if (!PyList_CheckExact(list))
    {
        throw std::invalid_argument("ComputeMultiple expects to get a list of pre-allocated FloatVector or DoubleVector or AutoDataVector buffers for all inputs and outputs");
    }
    size_t listSize = PyList_Size(list);

    size_t numInputs = map->NumInputs();

    if (numInputs != listSize)
    {
        throw std::invalid_argument(ell::utilities::FormatString("List argument is the wrong size, expecting %zu inputs", numInputs));
    }
        
    std::vector<void*> args;
    for (size_t i = 0; i < numInputs; ++i)
    {
        auto portType = map->GetInputType(i);
        switch (portType)
        {
        case ell::model::Port::PortType::smallReal:
            ExtractBufferFromPythonList<float>(map, list, i, args);
            break;
        case ell::model::Port::PortType::real:
            {
                auto item = PyList_GetItem(list, i);
                std::vector<double>* ptr = nullptr;
                auto res = swig::asptr(item, &ptr);
                if (!SWIG_IsOK(res)) 
                {                        
                    throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong type, expecting 'double*'", i));
                }
                auto argSize = map->GetInputSize(i);
                if (argSize != ptr->size())
                {
                    throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong size, expecting '%zu'", i, argSize));
                }
                args.push_back(ptr->data());
            }
            break;
        case ell::model::Port::PortType::integer:
            {
                auto item = PyList_GetItem(list, i);
                std::vector<int>* ptr = nullptr;
                auto res = swig::asptr(item, &ptr);
                if (!SWIG_IsOK(res)) 
                {                        
                    throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong type, expecting 'int*'", i));
                }
                auto argSize = map->GetInputSize(i);
                if (argSize != ptr->size())
                {
                    throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong size, expecting '%zu'", i, argSize));
                }
                args.push_back(ptr->data());
            }
            break;
        // case ell::model::Port::PortType::bigInt:
        //     {
        //         auto item = PyList_GetItem(list, i);
        //         std::vector<int64_t>* ptr = nullptr;
        //         auto res = swig::asptr(item, &ptr);
        //         if (!SWIG_IsOK(res)) 
        //         {                        
        //             throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong type, expecting 'int64_t*'", i));
        //         }
        //         auto argSize = map->GetInputSize(i);
        //         if (argSize != ptr->size())
        //         {
        //             throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong size, expecting '%zu'", i, argSize));
        //         }
        //         args.push_back(ptr->data());
        //     }
        //     break;
            // todo: doesn't compile on GCC and CLang
            throw std::invalid_argument(ell::utilities::FormatString("List argument %zu type PortType::bigInt is not yet supported'", i));
        case ell::model::Port::PortType::categorical:
            // todo
            throw std::invalid_argument(ell::utilities::FormatString("List argument %zu type PortType::categorical is not yet supported'", i));
        case ell::model::Port::PortType::boolean:
            {
                auto item = PyList_GetItem(list, i);
                std::vector<int8_t>* ptr = nullptr;
                auto res = swig::asptr(item, &ptr);
                if (!SWIG_IsOK(res)) 
                {                        
                    throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong type, expecting 'int8_t*'", i));
                }
                auto argSize = map->GetInputSize(i);
                if (argSize != ptr->size())
                {
                    throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong size, expecting '%zu'", i, argSize));
                }
                args.push_back(ptr->data());
            }
            break;
        default:
            throw std::invalid_argument(ell::utilities::FormatString("List argument %zu type has unsupported type'", i)); 
        }
    }
    
    return args;
}
    
std::vector<void*> GetOutputBuffersFromList(std::shared_ptr<ell::model::Map> map, PyObject *list) 
{
    // Here we expect a list of arguments, each one is a pre-allocated FloatVector or DoubleVector or AutoDataVector
    // for all the expected outputs of the model.
    if (!PyList_CheckExact(list))
    {
        throw std::invalid_argument("Compute expects to get a list of pre-allocated FloatVector or DoubleVector or AutoDataVector buffers for all inputs and outputs");
    }
    size_t listSize = PyList_Size(list);

    size_t numOutputs = map->NumOutputs();

    if (numOutputs != listSize)
    {
        throw std::invalid_argument(ell::utilities::FormatString("List argument is the wrong size, expecting %zu outputs", numOutputs));
    }
        
    std::vector<void*> args;
    for (size_t i = 0; i < numOutputs; ++i)
    {
        auto portType = map->GetOutputType(i);
        switch (portType)
        {
        case ell::model::Port::PortType::smallReal:
            {
                auto item = PyList_GetItem(list, i);
                std::vector<float>* ptr = nullptr;
                auto res = swig::asptr(item, &ptr);
                if (!SWIG_IsOK(res)) 
                {                        
                    throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong type, expecting 'float*'", i));
                }
                auto argSize = map->GetOutputSize(i);
                if (argSize != ptr->size())
                {
                    throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong size, expecting '%zu'", i, argSize));
                }
                args.push_back(ptr->data());
            }
            break;
        case ell::model::Port::PortType::real:
            {
                auto item = PyList_GetItem(list, i);
                std::vector<double>* ptr = nullptr;
                auto res = swig::asptr(item, &ptr);
                if (!SWIG_IsOK(res)) 
                {                        
                    throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong type, expecting 'double*'", i));
                }
                auto argSize = map->GetOutputSize(i);
                if (argSize != ptr->size())
                {
                    throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong size, expecting '%zu'", i, argSize));
                }
                args.push_back(ptr->data());
            }
            break;
        case ell::model::Port::PortType::integer:
            {
                auto item = PyList_GetItem(list, i);
                std::vector<int>* ptr = nullptr;
                auto res = swig::asptr(item, &ptr);
                if (!SWIG_IsOK(res)) 
                {                        
                    throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong type, expecting 'int*'", i));
                }
                auto argSize = map->GetOutputSize(i);
                if (argSize != ptr->size())
                {
                    throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong size, expecting '%zu'", i, argSize));
                }
                args.push_back(ptr->data());
            }
            break;
        //case ell::model::Port::PortType::bigInt:
        //    {
        //        auto item = PyList_GetItem(list, i);
        //        std::vector<int64_t>* ptr = nullptr;
        //        auto res = swig::asptr(item, &ptr);
        //        if (!SWIG_IsOK(res)) 
        //        {                        
        //            throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong type, expecting 'int64_t*'", i));
        //        }
        //        auto argSize = map->GetOutputSize(i);
        //        if (argSize != ptr->size())
        //        {
        //            throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong size, expecting '%zu'", i, argSize));
        //        }
        //        args.push_back(ptr->data());
        //    }
        //    break;
            // todo: there is no Swig BigIntVector wrapper...
            throw std::invalid_argument(ell::utilities::FormatString("List argument %zu type PortType::bigInt is not yet supported'", i));
        case ell::model::Port::PortType::categorical:
            // todo
            throw std::invalid_argument(ell::utilities::FormatString("List argument %zu type PortType::categorical is not yet supported'", i));
        case ell::model::Port::PortType::boolean:
            {
                auto item = PyList_GetItem(list, i);
                std::vector<int8_t>* ptr = nullptr;
                auto res = swig::asptr(item, &ptr);
                if (!SWIG_IsOK(res)) 
                {                        
                    throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong type, expecting 'int8_t*'", i));
                }
                auto argSize = map->GetOutputSize(i);
                if (argSize != ptr->size())
                {
                    throw std::invalid_argument(ell::utilities::FormatString("List argument %zu is the wrong size, expecting '%zu'", i, argSize));
                }
                args.push_back(ptr->data());
            }
        default:
            throw std::invalid_argument(ell::utilities::FormatString("List argument %zu type has unsupported type'", i)); 
        }
    }
    return args;
}

%}

namespace ELL_API
{
%extend Map 
{
    void ComputeMultiple(PyObject *inputList, PyObject *outputList) 
    {
        if (self->HasSourceNodes())
        {
            throw std::invalid_argument("Cannot use ComputeMultiple on a model with Source and Sink nodes");
        }
        auto map = self->GetInnerMap();
        std::vector<void*> inputs = GetInputBuffersFromList(map, inputList);
        std::vector<void*> outputs = GetOutputBuffersFromList(map, outputList);
        map->ComputeMultiple(inputs, outputs);
    }
}

%extend CompiledMap 
{
    void ComputeMultiple(PyObject *inputList, PyObject *outputList) 
    {
        if (self->HasSourceNodes())
        {
            throw std::invalid_argument("Cannot use ComputeMultiple on a model with Source and Sink nodes, use RegisterCallbacks and Step instead");
        }
        auto map = self->GetInnerMap();
        std::vector<void*> inputs = GetInputBuffersFromList(map, inputList);
        std::vector<void*> outputs = GetOutputBuffersFromList(map, outputList);
        self->GetInnerCompiledMap()->ComputeMultiple(inputs, outputs);
    }
}

}
