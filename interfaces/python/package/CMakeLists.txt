find_package(PythonInterp)
find_package(PythonLibs)
if(${PYTHONLIBS_FOUND})

    set(module_name "pythonpackage")
    
    set(src bld.bat build.sh MANIFEST.in meta.yaml setup.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${src})

    foreach(FILE ${src})
        configure_file(${FILE} ${FILE} COPYONLY)
    endforeach()

    set_property(TARGET ${module_name} PROPERTY FOLDER "interfaces/packages")
endif()

add_subdirectory(ell)
