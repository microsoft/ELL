import os
import subprocess

#binDir = os.path.abspath("C:/Users/OferD/Source/Repos/Bonsai/build/bin/Debug")
#dataDir = os.path.abspath("C:/Users/OferD/Source/Repos/Bonsai/data")
binDir = os.path.abspath("../bin/Debug")
dataDir = os.path.abspath("../../data")

#sgdBinary = os.path.join(binDir, "sgd.exe")
#printBinary = os.path.join(binDir, "print.exe")

sgdBinary = os.path.join(binDir, "sgd")
printBinary = os.path.join(binDir, "print")

trainingDataPath = os.path.join(dataDir, "a9a.txt")
outputMapFile1 = os.path.join(binDir, "output1.map")
outputMapFile2 = os.path.join(binDir, "output2.map")
outputSvgFile = os.path.join(binDir, "print.html")

subprocess.call([sgdBinary, '-idf', trainingDataPath, '-omf', outputMapFile1])
subprocess.call([sgdBinary, '-idf', trainingDataPath, '-imf', outputMapFile1, '-omf', outputMapFile2, '-imis', '2'])
subprocess.call([printBinary, '-imf', outputMapFile2, '-osf', outputSvgFile])
