import os

binDir = "C:/Users/OferD/Source/Repos/Bonsai/build/bin/Debug"
dataDir = "C:/Users/OferD/Source/Repos/Bonsai/data"

trainingDataPath = os.path.abspath(os.path.join(dataDir, "a9a.txt"))
outputMapFile1 = os.path.abspath(os.path.join(binDir, "output1.map"))
outputMapFile2 = os.path.abspath(os.path.join(binDir, "output2.map"))
outputSvgFile = os.path.abspath(os.path.join(binDir, "print.html"))

os.system("%s -idf %s -omf %s" %(os.path.join(binDir, "sgd.exe"), trainingDataPath, outputMapFile1))
os.system("%s -idf %s -imf %s -omf %s -imis 2" %(os.path.join(binDir, "sgd.exe"), trainingDataPath, outputMapFile1, outputMapFile2))
os.system("%s -imf %s -osf %s" %(os.path.join(binDir, "print.exe"), outputMapFile2, outputSvgFile))
