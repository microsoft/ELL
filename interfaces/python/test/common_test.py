from __future__ import print_function
import ELL

data = [ 
    #     key    label       file
    #            for         prefix
    #            print       for
    #                        saving
    #                        to disk
    (     "[1]", "Model 1", "model_1"), 
    (     "[2]", "Model 2", "model_2"), 
    (     "[3]", "Model 3", "model_3"),
    ("[tree_0]", "Tree 0",  "tree_0" ), 
    ("[tree_1]", "Tree 1",  "tree_1" ), 
    ("[tree_2]", "Tree 2",  "tree_2" ), 
    ("[tree_3]", "Tree 3",  "tree_3" )  ]

extensions = ["json"]

def TestModelSize(key, label):
    model = ELL.ELL_Model(key)
    print("%s size: %d" % (label, model.Size()))

def TestModelSizes():
    for (key, label, prefix) in data: 
        TestModelSize(key, label)

def TestLoadModel(fileName):
    print("Loading file %s" % fileName)
    m = ELL.ELL_LoadModel(fileName)

def TestLoadModels():
    prefix = "../../../examples/data/"
    fileNames = ["model_1.model", "model_2.model"]
    for fileName in fileNames:
        TestLoadModel(prefix + fileName)

def TestSaveModel(ext, key, prefix):
    fileName = prefix + "." + ext
    ELL.ELL_Model(key).Save(fileName)

def TestSaveModelsExt(ext):
    for (key, label, prefix) in data:
        TestSaveModel(ext, key, prefix)

def TestSaveModels():
    for ext in extensions:
        TestSaveModelsExt(ext)

def test():
    try:
        TestModelSizes()
        TestLoadModels()
        TestSaveModels()
        return  0
    except:
        return 1
        

