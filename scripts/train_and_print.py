from recipeUtil import *

dataFile = args[0]
stackFile1 = args[1]
stackFile2 = args[2]
outFile  = args[3]

runBinary('sgd', ['-idf', dataFile, '-osf', stackFile1])
runBinary('sgd', ['-idf', dataFile, '-isf', stackFile1, '-osf', stackFile2, '-cl', 'e-2'])

if outFile:
    runBinary('print', ['-isf', mapFile2, '-osvg', outFile])
