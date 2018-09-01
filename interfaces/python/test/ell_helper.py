import os
import sys
script_path = os.path.dirname(os.path.abspath(__file__))
sys.path += [
    os.path.join(script_path, '..', '..', '..', 'tools', 'utilities', 'pythonlibs'),
    '.',
    '..']
import find_ell