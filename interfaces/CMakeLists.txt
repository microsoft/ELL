#
# SWIG interfaces
#
include (CommonInterfaces)

set (INTERFACE_SRC ../common/src/DataLoadersInterface.cpp
                   ../common/src/LoadModelInterface.cpp
                   ../common/src/MapInterface.cpp
                   ../common/src/ModelInterface.cpp
                   ../common/src/RowDatasetInterface.cpp)

set (INTERFACE_INCLUDE ../common/include/DataLoadersInterface.h
                       ../common/include/LoadModelInterface.h
                       ../common/include/MapInterface.h
                       ../common/include/ModelInterface.h
                       ../common/include/RowDatasetInterface.h
                       ../common/include/SGDIncrementalTrainer_wrap.h)

set (INTERFACE_MAIN ../common/EMLL.i)

set (INTERFACE_FILES ../common/common.i
                     ../common/dataset.i
                     ../common/evaluators.i
                     ../common/features.i
                     ../common/layers.i
                     ../common/linear.i
                     ../common/lossFunctions.i
                     ../common/model.i
                     ../common/nodes.i
                     ../common/noncopyable.i
                     ../common/trainers.i
                     ../common/predictors.i
                     ../common/unique_ptr.i
                     ../common/utilities.i)

set (INTERFACE_DEPENDENCIES common dataset evaluators features layers linear lossFunctions trainers predictors testing treeLayout utilities)

# Add EMLL library include directories
include_directories(../libraries/common/include)
include_directories(../libraries/dataset/include)
include_directories(../libraries/evaluators/include)
include_directories(../libraries/features/include)
include_directories(../libraries/layers/include)
include_directories(../libraries/linear/include)
include_directories(../libraries/lossFunctions/include)
include_directories(../libraries/model/include)
include_directories(../libraries/nodes/include)
include_directories(../libraries/predictors/include)
include_directories(../libraries/trainers/include)
include_directories(../libraries/utilities/include)
include_directories(common/include)

add_subdirectory(common)
add_subdirectory(python)
# add_subdirectory(R) # currently doesn't compile because std_shared_ptr.i doesn't exist for R
add_subdirectory(csharp)
add_subdirectory(javascript)
add_subdirectory(xml) 

