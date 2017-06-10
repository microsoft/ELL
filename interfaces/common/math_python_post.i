%define CONSTRUCTABLE_WITH_NUMPY(TypeName)
%pythoncode %{
    class TypeName(TypeName):
        def __init__(self, numpyArray):
            if (len(numpyArray.shape) == 1):
                super(TypeName, self).__init__(numpyArray)
            elif (len(numpyArray.shape) == 3):
                super(TypeName, self).__init__(numpyArray.ravel(), numpyArray.shape[0], numpyArray.shape[1], numpyArray.shape[2])
            elif (len(numpyArray.shape) == 4):
                # Create a stacked 3 dimensional tensor 
                numpyArrayStacked = numpyArray.reshape(numpyArray.shape[0] * numpyArray.shape[1], numpyArray.shape[2], numpyArray.shape[3])
                super(TypeName, self).__init__(numpyArrayStacked.ravel(), numpyArrayStacked.shape[0], numpyArrayStacked.shape[1], numpyArrayStacked.shape[2])
            else:
                raise ValueError('Invalid number of dimensions!')
%}
%enddef

CONSTRUCTABLE_WITH_NUMPY(FloatVector)
CONSTRUCTABLE_WITH_NUMPY(FloatTensor)

CONSTRUCTABLE_WITH_NUMPY(DoubleVector)
CONSTRUCTABLE_WITH_NUMPY(DoubleTensor)
