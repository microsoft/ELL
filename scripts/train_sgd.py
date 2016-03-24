from recipeUtil import *

dataFile = args[0]
stackFile1 = args[1]
htmlOutFile = args[2]
codeOutFile = args[3]

# train using sgd
runBinary('sgd', ['-idf', dataFile, '-osf', stackFile1])

# print to svg file
runBinary('print', ['-isf', stackFile1, '-osvg', htmlOutFile])

# compile to C code
#runBinary('compile', ['-isf', stackFile1, '-ocf', codeOutFile])
