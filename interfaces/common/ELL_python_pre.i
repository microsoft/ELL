%pythonbegin %{

import os
__this_file_directory = os.path.dirname(os.path.abspath(__file__))


# Check if a directory looks like a valid ELL build directory
def __is_ell_build_directory(path):
    return os.path.isdir(os.path.join(path, 'interfaces')) and os.path.isdir(
        os.path.join(path, 'libraries')) and os.path.isfile(
            os.path.join(path, 'CMakeCache.txt'))


# Try to find the build directory for ELL
def __get_ell_build_dir():
    path = os.path.join(__this_file_directory, '..', '..')
    if __is_ell_build_directory(path):
        return os.path.abspath(path)
    return None


__ell_build_dir = __get_ell_build_dir()
if __ell_build_dir is None:
    parent_dir = os.path.basename(__this_file_directory)
    if parent_dir != 'ell': # if not the conda package
        print('warning: Unable to find ELL binaries')
else:
    import sys
    import os

    build_dir = __ell_build_dir
    interface_dir = os.path.join(build_dir, 'interfaces', 'python')
    bin_dir = os.path.join(build_dir, 'bin')

    sys.path.append(interface_dir)
    sys.path.append(os.path.join(interface_dir, 'utilities'))
    sys.path.append(os.path.join(interface_dir, 'Release'))
    sys.path.append(os.path.join(interface_dir, 'Debug'))
    sys.path.append(os.path.join(bin_dir, 'Release'))
    sys.path.append(os.path.join(bin_dir, 'Debug'))
    sys.path.append(os.path.join(build_dir, 'tools', 'importers', 'CNTK'))
    sys.path.append(os.path.join(build_dir, 'tools', 'importers', 'darknet'))

    # Add build/bin directory to system path, so OpenBLAS DLLs can be found
    if sys.platform == 'win32':
        os.environ['PATH'] = os.pathsep.join([
            os.environ['PATH'],
            os.path.join(bin_dir, 'Release'),
            os.path.join(bin_dir, 'Debug')
        ])
        
%}
