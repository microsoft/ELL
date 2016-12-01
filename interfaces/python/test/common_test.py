import EMLL

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

extensions = ["xml", "json"]

def TestModelSize(key, label):
    model = EMLL.ELL_Model(key)
    print "%s size: %d" % (label, model.Size())

def TestModelSizes():
    for (key, label, prefix) in data: 
        TestModelSize(key, label)

def TestLoadModel(fileName):
    print "Loading file %s" % fileName
    m = EMLL.ELL_LoadModel(fileName)

def TestLoadModels():
#    prefix = "../../examples/data/"
    prefix = "D:/GitProj/ID/EMLL/examples/data/"
    fileNames = ["model_1.json", "model_2.json"]
    for fileName in fileNames:
        TestLoadModel(prefix + fileName)

def TestSaveModel(ext, key, prefix):
    fileName = prefix + "." + ext
    EMLL.ELL_Model(key).Save(fileName)

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
        

