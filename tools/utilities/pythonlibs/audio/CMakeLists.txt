#
# cmake file for python utilities
#

if(${PYTHON_ENABLED})

    set(module_name "pythonlibs-audio")

    set(lib_src 
        classifier.py
        compiled_ell_model.py
        compute_ell_model.py
        eval_test.py
        featurizer.py
        microphone.py
        record.py
        speaker.py
        test_audio.py
        view_audio.py
        wav_reader.py
    )

    add_custom_target(${module_name} DEPENDS SOURCES ${lib_src})

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/utilities/audio")
    
    # copy files
    foreach(PY_FILE ${lib_src})
        configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
    endforeach()

endif()  # PYTHON_ENABLED
