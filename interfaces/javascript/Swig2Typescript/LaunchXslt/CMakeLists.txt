#
# SWIG javascript-on-node interfaces
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

# define project
set (PROJ_NAME LaunchXslt)
PROJECT(${PROJ_NAME})

set (CSPROJ_FILES 
    App.config
    CodeFromXsltScript.cs
    generate-d-ts.xlst
    LaunchXslt.csproj
    Program.cs
    Properties/AssemblyInfo.cs
)

add_custom_target(${PROJ_NAME}_files ALL
    DEPENDS ${CSPROJ_FILES} 
    COMMENT "Copying LaunchXslt files"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

foreach(CSPROJ_FILE ${CSPROJ_FILES})
    configure_file(${CSPROJ_FILE} ${CSPROJ_FILE} COPYONLY)
    add_custom_command(OUTPUT ${CSPROJ_FILE} 
                    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${CSPROJ_FILE} ${CMAKE_CURRENT_BINARY_DIR}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Copying ${CSPROJ_FILE}"
                    VERBATIM)
    set_property(TARGET ${PROJ_NAME}_files APPEND PROPERTY OBJECT_DEPENDS ${CSPROJ_FILE})
endforeach()

include_external_msproject(${PROJ_NAME} ${CMAKE_CURRENT_BINARY_DIR}/LaunchXslt.csproj 
    TYPE FAE04EC0-301F-11D3-BF4B-00C04F79EFBC
    PLATFORM "Any CPU")

# LaunchXslt\bin\Release\LaunchXslt.exe ..\..\..\build\interfaces\xml\EMLLXML_wrap.xml generate-d-ts.xlst ..\emll_module\emll.d.ts
add_custom_target(${PROJ_NAME}_run ALL
                COMMAND bin/Release/LaunchXslt.exe ../../../xml/EMLLXML_wrap.xml generate-d-ts.xlst ../../emll_module/emll.d.ts
                DEPENDS ${PROJ_NAME}_files EMLL_xml
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Generating Typescript .d.ts files"
                VERBATIM)
