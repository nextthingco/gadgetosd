CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
 
PROJECT("Gadget")

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

SET( PROJECT_VERSION_MINOR "0" )
SET( PROJECT_VERSION_MAJOR "0" )
SET( PROJECT_VERSION_PATCH "0" )
SET( PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" )

MESSAGE( "CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}" )
MESSAGE( "CMAKE_ROOT=${CMAKE_ROOT}" )
MESSAGE( "CMAKE_SYSTEM=${CMAKE_SYSTEM}" )

# Required for Mongoose to support file upload
ADD_DEFINITIONS( -DMG_ENABLE_HTTP_STREAMING_MULTIPART )

SET( GOSD_SRCS 
mongoose.c
ini.c
utils.c 
config.c 
mongoose_utils.c 
gadgetosd.c
gadgetosd_api_version.c 
gadgetosd_api_application_add.c
gadgetosd_api_application_start.c
gadgetosd_api_application_stop.c
gadgetosd_api_application_delete.c
gadgetosd_api_application_purge.c
)
SET( GOSD_EXECUTABLE gadgetosd )
ADD_EXECUTABLE("${GOSD_EXECUTABLE}" ${GOSD_SRCS} )
INSTALL(TARGETS ${GOSD_EXECUTABLE} DESTINATION bin)

SET( GADGET_SRCS 
mongoose.c
ini.c
utils.c 
config.c 
mongoose_utils.c 
gadget_project.c
gadget.c
gadget_init.c
gadget_build.c
gadget_stop.c
gadget_start.c
gadget_delete.c
gadget_purge.c
gadget_deploy.c
)
SET( GADGET_EXECUTABLE gadget )
ADD_EXECUTABLE("${GADGET_EXECUTABLE}" ${GADGET_SRCS} )
INSTALL(TARGETS ${GADGET_EXECUTABLE} DESTINATION bin)


SET (SVG_ICON_FILE "${CMAKE_SOURCE_DIR}/icon.svg")

SET( CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}" )
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  MESSAGE(STATUS "Running Mac packaging")
  INCLUDE("mac/packaging.cmake")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  MESSAGE(STATUS "Running Linux packaging")
  INCLUDE("ubuntu/packaging.cmake")
ENDIF()

INCLUDE( CPack )
