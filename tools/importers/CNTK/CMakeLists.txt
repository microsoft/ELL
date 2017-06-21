#
# cmake file
#

find_package(PythonInterp)
if(${PYTHONINTERP_FOUND} AND BUILD_INTERFACES)

	set(module_name "cntk_importer")

	set(importer_src cntk_to_ell.py)

	add_custom_target(${module_name} ALL DEPENDS SOURCES ${importer_src})

	set_property(TARGET ${module_name} PROPERTY FOLDER "tools/importers")
	#copy files
	foreach(PY_FILE ${importer_src})
		configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
	endforeach()

	add_subdirectory(test)
endif()  # PYTHONLIBS_FOUND
