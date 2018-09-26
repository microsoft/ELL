if(${PYTHON_ENABLED})

    set(module_name "pythonpackage")
    
    set(src bld.bat build.sh MANIFEST.in meta.yaml setup.py)

    add_custom_target(${module_name} ALL DEPENDS SOURCES ${src})
    
    copy_newer_files(${module_name} src)

    set_property(TARGET ${module_name} PROPERTY FOLDER "interfaces/python/package")

endif()  # PYTHON_ENABLED

add_subdirectory(ell)
