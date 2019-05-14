#
# cmake file for python optimizer utilities
#

if(${PYTHON_ENABLED})

    set(module_name "optimizer")

    set(lib_src
        optimizer_util.py
        profile_builder.py
        profile_runner.py
        profile_optimizer.py
        profile_and_optimize.py
        recover_convolutions.py
        json_profile_optimizer.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${lib_src})

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/utilities")

    # copy files
    copy_newer_files(${module_name} lib_src)

    # run flake8
    flake8(${module_name})

endif()