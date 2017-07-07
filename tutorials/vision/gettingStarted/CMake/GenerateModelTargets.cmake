#
# GenerateModelTargets
#

# Creates a target to compile an ELL model for an architecture
function(generate_ell_model_compile_target model_name arch_name target_name demo_files)

    find_program(LLC_EXECUTABLE llc HINTS ${LLVM_TOOLS_BINARY_DIR})
    if(NOT LLC_EXECUTABLE)
        message(ERROR "LLVM not found, please check that LLVM is installed.")
        return()
    endif()

    # arch processing
    if(arch_name STREQUAL "pi3") # Raspberry Pi 3
        set(llc_options -O3 -mtriple=armv7-linux-gnueabihf -mcpu=cortex-a53 -relocation-model=pic)
    else() # host
        set(llc_options -O3 -relocation-model=pic)
    endif()

    # cmake target and output
    set(target_path ${CMAKE_CURRENT_BINARY_DIR}/compiled_${model_name}_${arch_name})

    set(compiled_output
        ${target_path}/${model_name}.ll
        ${target_path}/${model_name}.i
        ${target_path}/${model_name}.i.h)
    set(llc_output ${target_path}/${model_name}.${OBJECT_EXTENSION})
    set(swig_output
        ${target_path}/${model_name}PYTHON_wrap.cxx
        ${target_path}/${model_name}PYTHON_wrap.h
        ${target_path}/_${model_name}.py)
    set(target_output ${compiled_output} ${llc_output} ${swig_output})

    # run commands to generate model files and invoke SWIG

    # ELL's compile tool
    set(compile_options --blas)
    add_custom_command(
        OUTPUT ${compiled_output}
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${model_name}.map
        COMMAND compile -imap ${CMAKE_CURRENT_BINARY_DIR}/${model_name}.map -cfn predict -cmn ${model_name} -od ${target_path} --ir --swig ${compile_options}
        COMMENT "Generating ${compiled_output}")

    # llc
    add_custom_command(
        OUTPUT ${llc_output}
        DEPENDS ${target_path}/${model_name}.ll
        COMMAND ${CMAKE_COMMAND} -E make_directory ${target_path}
        COMMAND ${LLC_EXECUTABLE} ${target_path}/${model_name}.ll -o ${target_path}/${model_name}.${OBJECT_EXTENSION} -filetype=obj ${llc_options}
        COMMAND ${CMAKE_COMMAND} -E remove ${target_path}/${model_name}.ll
        COMMENT "Compiling ${model_name}.ll to ${llc_output} for ${arch_name}")

    # swig
    set(ell_root ${CMAKE_CURRENT_SOURCE_DIR}/../../../)
    set(swig_options -python -c++ -Fmicrosoft -py3 -o ${target_path}/${model_name}PYTHON_wrap.cxx)
    set(swig_includes
        -I${ell_root}interfaces/common/include
        -I${ell_root}interfaces/common
        -I${ell_root}libraries/emitters/include)
    add_custom_command(
        OUTPUT ${swig_output}
        DEPENDS ${compiled_output}
        COMMAND ${SWIG_EXECUTABLE} ${swig_options} ${swig_includes} -outdir ${target_path} ${target_path}/${model_name}.i
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ell_root}interfaces/common/include/CallbackInterface.h ${target_path}/include/CallbackInterface.h
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ell_root}interfaces/common/tcc/CallbackInterface.tcc ${target_path}/tcc/CallbackInterface.tcc
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ell_root}libraries/emitters/include/ClockInterface.h ${target_path}/include/ClockInterface.h
        COMMENT "Generating SWIG wrappers for ${model_name}")

    # configure CMake files for compiling on the target
    set (ELL_model ${model_name})
    set (Arch ${arch_name})
    configure_file(CMake/CMakeLists.txt.in ${target_path}/CMakeLists.txt @ONLY)
    configure_file(${CMAKE_SOURCE_DIR}/CMake/OpenBLASSetup.cmake ${target_path}/OpenBLASSetup.cmake COPYONLY)
    configure_file(setup.py.in ${target_path}/${model_name}_setup.py)

    # collect the demo files
    foreach (demo_file ${demo_files})
        configure_file(${demo_file} ${target_path}/${demo_file})
    endforeach()
    configure_file(modelHelper.py ${target_path}/modelHelper.py)

    # create our custom target that kicks everything off
    add_custom_target(${target_name} DEPENDS ${target_output})
    set_source_files_properties(${target_output} PROPERTIES GENERATED TRUE)
    set_property(TARGET ${target_name} PROPERTY EXCLUDE_FROM_ALL TRUE)

    message(STATUS "Generated target ${target_name}")
endfunction()