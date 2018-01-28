import ell
import os

def TestLoadSaveModel(fileName):
    print("Loading file %s" % fileName)
    m = ell.model.Model(fileName)

    # save file in build folder to ensure we don't cause git changes
    fname = os.path.basename(fileName)
    path = os.path.abspath(fname)
    print("saving model in '%s'" % (path))
    m.Save(path)

def TestLoadSaveModels():
    prefix = "../../../examples/models/"
    fileNames = ["model_1.model", "model_2.model"]
    for fileName in fileNames:
        TestLoadSaveModel(prefix + fileName)


def test():
    try:
        TestLoadSaveModels()
        return  0
    except:
        return 1


