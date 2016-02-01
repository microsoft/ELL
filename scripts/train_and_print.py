import os
import subprocess

def padList(l, length):
    if len(l) < length:
        return l + [None]*(length-len(l))
    else:
        return l[:length]

def findDirWithFile(dirs, filenames):
    '''Of the supplied list of directories, returns the first one that contains a file in the list filenames'''
    for dir in dirs:
        for filename in filenames:
            if os.path.isfile(os.path.join(dir, filename)):
                return dir
    return None

def main(trainingDataFile, mapFile1, mapFile2, outputSvgFile=None):
    binDir = findDirWithFile(['../bin/Debug', '../bin/Release', '../bin'], ['sgd', 'sgd.exe'])
    print binDir

    sgdBinary = os.path.join(binDir, 'sgd')
    printBinary = os.path.join(binDir, 'print')
    dataDir = os.path.abspath('../../data')
    trainingDataPath = os.path.join(dataDir, trainingDataFile)
    outputMapFile1 = os.path.join(binDir, mapFile1)
    outputMapFile2 = os.path.join(binDir, mapFile2) 

    subprocess.check_call([sgdBinary, '-idf', trainingDataPath, '-omf', outputMapFile1])
    subprocess.check_call([sgdBinary, '-idf', trainingDataPath, '-imf', outputMapFile1, '-omf', outputMapFile2, '-clis', '2'])
    if outputSvgFile:
        outputSvgFile = os.path.join(binDir, outputSvgFile)
        subprocess.check_call([printBinary, '-imf', outputMapFile2, '-osf', outputSvgFile])

if __name__ == '__main__':
    import sys
    dataFile, mapFile1, mapFile2, outFile = padList(sys.argv[1:], 4)
    try:
        main(dataFile, mapFile1, mapFile2, outFile)
    except:
        sys.exit(1)
