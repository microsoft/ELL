####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     extractor.py
##  Authors:  Lisa Ong
##
##  Requires: Python 3.x
##
####################################################################################################

import os
from os.path import basename
import zipfile

class Extractor:
    def __init__(self, archive):
        self.archive = archive
        return

    def extract_file(self, file_extension):
        """Extracts the first file matching the file extension"""
        _, ext = os.path.splitext(basename(self.archive))
        if (ext == ".zip"):
            with zipfile.ZipFile(self.archive) as zf:
                for member in zf.infolist():
                    _, e = os.path.splitext(member.filename)
                    if (e == file_extension):
                        path = os.path.dirname(self.archive)
                        zf.extract(member, path)
                        return True, os.path.join(path, member.filename)
        else:
            return False, ""