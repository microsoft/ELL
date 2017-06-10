#
# cmake file
#

find_package(PythonInterp)
find_package(PythonLibs)
if(${PYTHONLIBS_FOUND})
	# test project

	set (test_name ${module_name}_test)

	set (test_src cntk_to_ell_importer_test.py)

	set (test_data xorModel1.dnn)

	set (main_file cntk_to_ell_importer_test.py)

	add_custom_target(${test_name} ALL DEPENDS COMMAND python ${main_file} SOURCES ${test_src})

	set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

	#copy files
	foreach(PY_FILE ${test_src})
		configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
	endforeach()
	foreach(PY_FILE ${test_data})
		configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
	endforeach()

	add_test(NAME ${test_name} COMMAND python -m unittest ${test_src})
endif()
