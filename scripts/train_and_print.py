from recipeUtil import *

dataFile = args[0]
mapFile1 = args[1]
mapFile2 = args[2]
outFile = args[3]

print '####'
print os.getcwd()
print dataFile, mapFile1, mapFile2, outFile
print '####'

callBinary('sgd', ['-idf', dataFile, '-omf', mapFile1])
callBinary('sgd', ['-idf', dataFile, '-imf', mapFile1, '-omf', mapFile2, '-imis', '2'])
if outputSvgFile:
    callBinary('print', ['-imf', mapFile2, '-osf', outFile])

#try:
#    train_and_print(args.dataFile, args.mapFile1, args.mapFile2, args.outFile)
#except:
#    sys.exit(1)
