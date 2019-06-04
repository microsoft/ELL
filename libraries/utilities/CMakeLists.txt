#
# cmake file
#

set(library_name utilities)

set(src
  src/Archiver.cpp
  src/ArchiveVersion.cpp
  src/Boolean.cpp
  src/CommandLineParser.cpp
  src/CompressedIntegerList.cpp
  src/CStringParser.cpp
  src/Files.cpp
  src/Format.cpp
  src/Graph.cpp
  src/IArchivable.cpp
  src/IndentedTextWriter.cpp
  src/IntegerList.cpp
  src/IntegerStack.cpp
  src/JsonArchiver.cpp
  src/Logger.cpp
  src/MemoryLayout.cpp
  src/MillisecondTimer.cpp
  src/ObjectArchive.cpp
  src/ObjectArchiver.cpp
  src/OutputStreamImpostor.cpp
  src/PPMImageParser.cpp
  src/PropertyBag.cpp
  src/RandomEngines.cpp
  src/StringUtil.cpp
  src/Tokenizer.cpp
  src/TypeName.cpp
  src/UniqueId.cpp
  src/UniqueNameList.cpp
  src/Variant.cpp
  src/XmlArchiver.cpp
)

set(include
  include/AbstractInvoker.h
  include/AnyIterator.h
  include/Archiver.h
  include/ArchiveVersion.h
  include/Boolean.h  
  include/CallbackRegistry.h
  include/CommandLineParser.h
  include/CompressedIntegerList.h
  include/CStringParser.h
  include/Debug.h
  include/Graph.h
  include/Exception.h
  include/Files.h
  include/Format.h
  include/FunctionUtils.h
  include/Hash.h
  include/IArchivable.h
  include/IIterator.h
  include/IndentedTextWriter.h
  include/IntegerList.h
  include/IntegerNArray.h
  include/IntegerStack.h
  include/JsonArchiver.h
  include/Logger.h
  include/MemoryLayout.h
  include/MillisecondTimer.h
  include/ObjectArchive.h
  include/ObjectArchiver.h
  include/Optional.h
  include/OutputStreamImpostor.h
  include/ParallelTransformIterator.h
  include/PropertyBag.h
  include/PPMImageParser.h
  include/RandomEngines.h
  include/RingBuffer.h
  include/StlContainerIterator.h
  include/StlStridedIterator.h
  include/StlVectorUtil.h
  include/StringUtil.h
  include/Tokenizer.h
  include/TransformIterator.h
  include/TupleUtils.h
  include/TypeFactory.h
  include/TypeName.h
  include/TypeTraits.h
  include/UniqueId.h
  include/UniqueNameList.h
  include/Unused.h
  include/Variant.h
  include/XmlArchiver.h
  include/ZipIterator.h
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})

add_library(${library_name} ${src} ${include})
target_include_directories(${library_name} PRIVATE include ${ELL_LIBRARIES_DIR})
target_link_libraries(${library_name} Threads::Threads)

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test projects
#

#
# main tests
#
set (test_name ${library_name}_test)

set(test_src
  test/src/main.cpp
  test/src/Format_test.cpp
  test/src/FunctionUtils_test.cpp
  test/src/Archiver_test.cpp
  test/src/Hash_test.cpp
  test/src/Iterator_test.cpp
  test/src/MemoryLayout_test.cpp
  test/src/ObjectArchive_test.cpp
  test/src/PropertyBag_test.cpp
  test/src/RingBuffer_test.cpp
  test/src/TypeFactory_test.cpp
  test/src/TypeName_test.cpp
  test/src/Variant_test.cpp
  test/src/Files_test.cpp
)

set(test_include
  test/include/Format_test.h
  test/include/FunctionUtils_test.h
  test/include/Archiver_test.h
  test/include/Hash_test.h
  test/include/Iterator_test.h
  test/include/MemoryLayout_test.h
  test/include/ObjectArchive_test.h
  test/include/PropertyBag_test.h
  test/include/RingBuffer_test.h
  test/include/TypeFactory_test.h
  test/include/TypeName_test.h
  test/include/Variant_test.h
  test/include/Files_test.h
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include})
target_include_directories(${test_name} PRIVATE test/include ${ELL_LIBRARIES_DIR})

target_link_libraries(${test_name} utilities testing)
target_link_libraries(${test_name} Threads::Threads)

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")
add_test(NAME ${test_name} COMMAND ${test_name})
