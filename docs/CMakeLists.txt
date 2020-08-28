
find_package(Doxygen REQUIRED)

# Parameters to be replaced in Doxifile.in as "@VAR@"
file(GLOB_RECURSE DOXYGEN_INPUT_FILES_RAW
    ${PROJECT_SOURCE_DIR}/src/include/kompute/*.hpp)
# Need to do a string replace as files have to be 
# space separated and with double quotes
string(REPLACE ";" "\" \"" 
    DOXYGEN_INPUT_FILES "${DOXYGEN_INPUT_FILES_RAW}")
set(DOXYGEN_OUTPUT_DIR 
    ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
# DOXIGEN_DOT_PATH is also set automatically

# Parameters only used inside cmake script
set(DOXYGEN_INDEX_FILE
    ${DOXYGEN_OUTPUT_DIR}/html/index.html)
set(DOXYGEN_XML_PATH
    ${DOXYGEN_OUTPUT_DIR}/xml/)
set(DOXYFILE_IN
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Perform replacement with cmake vars inside Doxifine.in
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})
add_custom_command(
    OUTPUT ${DOXYGEN_INDEX_FILE}
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
    MAIN_DEPENDENCY ${DOXYFILE_IN} ${DOXYFILE_OUT}
    COMMENT "Generating docs"
)

add_custom_target(gendoxygen ALL 
    DEPENDS ${DOXYGEN_INDEX_FILE})


#####################################################
########### Sphinx ###############
#####################################################

find_package(Sphinx REQUIRED)

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)

add_custom_target(gensphinx ALL
    COMMAND
    ${SPHINX_EXECUTABLE} -b html 
        ${SPHINX_SOURCE} ${SPHINX_BUILD}
        -Dbreathe_projects.Kompute=${DOXYGEN_XML_PATH}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${DOXYGEN_INDEX_FILE}
    COMMENT "Generating documentation with Sphinx")

