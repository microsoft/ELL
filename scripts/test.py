import os

workingDirectory = "c:/tmp"
libraryRoot = "C:/Users/OferD/Source/Repos/Bonsai"

trainerExecutablePath = os.path.abspath(os.path.join(libraryRoot, "trainers/sgd/bin/debug/sgd.exe"))
trainingDataPath = os.path.abspath(os.path.join(libraryRoot, "data/a9a.txt"))
outputMapFile1 = os.path.abspath(os.path.join(workingDirectory, "output1.map"))
outputMapFile2 = os.path.abspath(os.path.join(workingDirectory, "output2.map"))

os.system("%s -df %s -omf %s" %(trainerExecutablePath, trainingDataPath, outputMapFile1))
os.system("%s -df %s -imf %s -omf %s -imis 2" %(trainerExecutablePath, trainingDataPath, outputMapFile1, outputMapFile2))
