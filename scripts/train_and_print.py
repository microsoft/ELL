from recipeUtil import *

dataFile = args[0]
mapFile1 = args[1]
mapFile2 = args[2]
outFile  = args[3]

callBinary('sgd', ['-idf', dataFile, '-omf', mapFile1])
callBinary('sgd', ['-idf', dataFile, '-imf', mapFile1, '-omf', mapFile2, '-imis', '2'])

if outFile:
    callBinary('print', ['-imf', mapFile2, '-osf', outFile])
