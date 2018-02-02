"""The various platforms to which ELL models can be compiled"""

MAC = 'mac'
LINUX = 'linux'
WINDOWS = 'windows'
PI0 = 'pi0'
PI3 = 'pi3'
PI3_64 = 'pi3_64'
AARCH64 = 'aarch64'
IOS = 'ios'

# special global to record the fact we are running inside Jupyter Notebook.
has_magic = False