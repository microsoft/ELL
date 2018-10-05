#
# cmake file for python audio utilities
#

if(${PYTHON_ENABLED})

    set(module_name "pythonlibs-audio")

    set(lib_src 
        classifier.py
        compiled_ell_model.py
        compute_ell_model.py
        featurizer.py
        microphone.py
        play_audio.py
        record.py
        speaker.py
        test_audio.py
        vad.py
        vad_test.py
        view_audio.py
        wav_reader.py
    )

    add_custom_target(${module_name} ALL DEPENDS SOURCES ${lib_src})
    add_dependencies(${module_name} pythonlibs)

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/utilities/audio")
    
    # copy files
    copy_newer_files(${module_name} lib_src)

    add_subdirectory(training)

endif()  # PYTHON_ENABLED
