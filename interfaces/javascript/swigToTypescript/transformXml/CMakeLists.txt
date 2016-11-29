#
# transformXml project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

# define project
set (PROJ_NAME transformXml)
PROJECT(${PROJ_NAME})

include_external_msproject(${PROJ_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/TransformXml.csproj 
    TYPE FAE04EC0-301F-11D3-BF4B-00C04F79EFBC
    PLATFORM "Any CPU")
