#
# cmake file for python audio training utilities
#

if(${PYTHON_ENABLED})

    set(module_name "pythonlibs-audio-training")

    set(lib_src 
        add_vad.py
        dataset.py
        eval_test.py
        featurizer.png
        find_ell_root.py
        make_dataset.py
        make_featurizer.py
        make_training_list.py
        process.dgml
        process.jpg
        readme.md
        train_classifier.py
    )

    add_custom_target(${module_name} ALL DEPENDS SOURCES ${lib_src})
    add_dependencies(${module_name} pythonlibs)

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/utilities/audio/training")
    
    # copy files
    copy_newer_files(${module_name} lib_src)

endif()  # PYTHON_ENABLED
