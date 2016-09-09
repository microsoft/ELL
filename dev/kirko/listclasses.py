# listclasses.py
#
# Run this in a directory that contains _EMLL.pyd and EMLL.py
# It will list all the exported EMLL function, classes and methods.
#
#   $ python listclasses.py > classlist.txt
#

import EMLL
import os
import sys

def contains(s,substr): return not s.find(substr) == -1
def iscapitalized(s): return s[:1].isupper()
def isexport(s): return iscapitalized(s) and (not contains(s,'_'))

exports = filter(isexport, dir(EMLL))
tempFileName = "deleteme.py"
if os.path.isfile(tempFileName):
    print "\"%s\" already exists -- please delete it and rerun" % (tempFileName)
    sys.exit(1)

f = open(tempFileName, 'w')
for x in exports:
    s = "print \"EMLL.%s\"" % (x)
    f.write(s + "\r\n")
    s = "exports = filter(isexport, dir(EMLL.%s))" % (x)
    f.write(s + "\r\n")
    s = "for x in exports:"
    f.write(s + "\r\n")
    s = "    print \"   \" , x"
    f.write(s + "\r\n")
f.close()

execfile(tempFileName)

