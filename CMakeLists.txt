set( MY_LIB_NAME tinyini )

SET( SRC_FILES ${SRC_FILES} include/tinyini/tinyini.h )
SET( SRC_FILES ${SRC_FILES} src/tinyini.cpp )

INCLUDE_DIRECTORIES( ${MY_LIB_NAME} include/tinyini )

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )