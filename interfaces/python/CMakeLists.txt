#
# SWIG python interfaces
#
find_package(PythonInterp 3.5 QUIET)
find_package(PythonLibs 3.5 QUIET)
if(${PYTHONLIBS_FOUND})
  add_subdirectory(test)
else()
  message(WARNING "=Couldn't find Python libraries 3.5 or later, _ELL_python target will not be build properly. This is not a required component, so it can be skipped.")

  set(PYTHON_LIBRARIES common)
  set(PYTHON_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
endif() # PYTHONLIBS_FOUND

add_subdirectory(package)
add_subdirectory(utilities)

include (CommonInterfaces)

include_directories(SYSTEM ${PYTHON_INCLUDE_PATH})

generate_interface(python ${CMAKE_CURRENT_SOURCE_DIR} ${PYTHON_LIBRARIES} "" ${PYTHON_INCLUDE_PATH})
set(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS})

if (_ELL_python)
  set(PYTHON_DIR ${CMAKE_BINARY_DIR}/interfaces/python)
  set(COMMON_DIR ${CMAKE_BINARY_DIR}/interfaces/common)

  # Copy the ELL headers and library to the right place in the package

  add_custom_command(TARGET _ELL_python
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${PYTHON_DIR}/ELL.py ${PYTHON_DIR}/package/ell/ELL.py
  )

  file(GLOB PKGHDR RELATIVE ${CMAKE_SOURCE_DIR}/interfaces ${CMAKE_SOURCE_DIR}/interfaces/common/*.i ${CMAKE_SOURCE_DIR}/interfaces/common/include/*.h)

  foreach(hdr ${PKGHDR})
    add_custom_command(TARGET _ELL_python
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/interfaces/${hdr} ${CMAKE_BINARY_DIR}/interfaces/python/package/ell/headers/${hdr}
    )
  endforeach()

  set(DEPLOYDIR ${CMAKE_BINARY_DIR}/interfaces/python/package/ell/deploy)

  add_custom_command(TARGET _ELL_python
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/CMake/OpenBLASSetup.cmake ${DEPLOYDIR}/OpenBLASSetup.cmake
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/interfaces/common/include/CallbackInterface.h ${DEPLOYDIR}/include/CallbackInterface.h
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/interfaces/common/tcc/CallbackInterface.tcc ${DEPLOYDIR}/tcc/CallbackInterface.tcc
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/tools/wrap/templates/CMakeLists.python.txt.in ${DEPLOYDIR}/CMakeLists.python.txt.in
  )

  if(WIN32)
    set(binaries _ELL.exp _ELL.lib _ELL.pyd)
    foreach(b ${binaries})
      add_custom_command(TARGET _ELL_python
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${PYTHON_DIR}/$<CONFIG>/${b} ${PYTHON_DIR}/package/ell/$<CONFIG>/${b}
      )
    endforeach()
  else()
    add_custom_command(TARGET _ELL_python
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:_ELL_python> ${PYTHON_DIR}/package/ell/$<TARGET_FILE_NAME:_ELL_python>
    )
  endif()
endif(_ELL_python)
