#
# SWIG python interfaces
#
if(${PYTHON_ENABLED})
    add_subdirectory(test)
else()
    message(WARNING "Couldn't find Python libraries 3.5 or later, _ELL_python target will not be build properly. This is not a required component, so it can be skipped.")

    set(PYTHON_LIBRARIES common)
    set(PYTHON_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
endif() # PYTHON_ENABLED

add_subdirectory(package)

set(INTERFACE_MAIN "${CMAKE_CURRENT_LIST_DIR}/../common/ell_py.i")

include (CommonInterfaces)

include_directories(SYSTEM ${PYTHON_INCLUDE_PATH})

generate_interface(python ell_py ${CMAKE_CURRENT_SOURCE_DIR} ${PYTHON_LIBRARIES} "" ${PYTHON_INCLUDE_PATH})
set(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS})

if (TARGET _ELL_python)
  set(PYTHON_DIR ${CMAKE_BINARY_DIR}/interfaces/python)
  set(COMMON_DIR ${CMAKE_BINARY_DIR}/interfaces/common)

  # Copy the ELL headers and library to the right place in the package

  add_custom_command(TARGET _ELL_python
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${PYTHON_DIR}/ell_py.py ${PYTHON_DIR}/package/ell/ell_py.py
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/tools/utilities/pythonlibs/buildtools.py ${PYTHON_DIR}/package/ell/util/buildtools.py
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/docs/tutorials/shared/tutorial_helpers.py ${PYTHON_DIR}/package/ell/util/tutorialHelpers.py
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
    set(binaries _ell_py.exp _ell_py.lib _ell_py.pyd)
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
endif(TARGET _ELL_python)