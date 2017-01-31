#
# cmake file
#

set (library_name model)

set (src src/DynamicMap.cpp
         src/InputNode.cpp
         src/Model.cpp
         src/ModelTransformer.cpp
         src/Node.cpp
         src/OutputNode.cpp
         src/OutputPort.cpp
         src/PortElements.cpp
         src/Port.cpp
)

set (include include/DynamicMap.h
             include/InputNode.h
             include/InputPort.h
             include/Model.h
             include/ModelTransformer.h
             include/Node.h
             include/OutputNode.h
             include/OutputPort.h
             include/PortElements.h
             include/Port.h
)

set (tcc tcc/DynamicMap.tcc
         tcc/InputNode.tcc
         tcc/InputPort.tcc
         tcc/Model.tcc
         tcc/ModelTransformer.tcc
         tcc/OutputNode.tcc
         tcc/OutputPort.tcc
         tcc/PortElements.tcc
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

add_library(${library_name} ${src} ${include} ${tcc})
target_include_directories(${library_name} PUBLIC include)
if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${library_name} PRIVATE -fPIC)
endif()

target_link_libraries(${library_name} utilities data)

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test project
#

set (test_name ${library_name}_test)

set (test_src test/src/main.cpp
              test/src/DynamicMap_test.cpp
              test/src/Model_test.cpp
              test/src/PortElements_test.cpp)

set (test_include test/include/PortElements_test.h
                  test/include/DynamicMap_test.h
                  test/include/Model_test.h)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include} ${include})
target_include_directories(${test_name} PRIVATE test/include)
target_link_libraries(${test_name} common model nodes testing utilities)
copy_shared_libraries(${test_name} $<TARGET_FILE_DIR:${test_name}>)

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
