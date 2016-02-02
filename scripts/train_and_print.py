from recipeUtil import *

dataFile = args[0]
mapFile1 = args[1]
mapFile2 = args[2]
outFile  = args[3]

runBinary('sgd', ['-idf', dataFile, '-omf', mapFile1])
runBinary('sgd', ['-idf', dataFile, '-imf', mapFile1, '-omf', mapFile2, '-clis', '2'])

if outFile:
    runBinary('print', ['-imf', mapFile2, '-osf', outFile])
