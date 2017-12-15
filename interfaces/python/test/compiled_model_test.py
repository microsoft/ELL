from __future__ import print_function
import functools
import ell
import os

def test():
    # Load the map created by proton trainer test and compile it
    map = ell.model.Map("protonnTestData.ell")
    compiledMap = map.Compile("host", "protonn", "predict", False)
    compiledMap.WriteBitcode("protonnTestData.bc");
    
    if os.path.isfile("protonnTestData.bc"):
        return 0

    print("### compiled_model_test failed to generate bitcode: protonnTestData.bc") 
    return 1


if __name__ == '__main__':
    test()
