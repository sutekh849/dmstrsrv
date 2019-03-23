file(GLOB_RECURSE dmstrsrv_SRCS *.cpp *.h)

set(dmstrsrv_SRCS
    ${dmstrsrv_SRCS}
    ${TEMPLATES_SRC}
)

# Create the application
add_library(dmstrsrv SHARED ${dmstrsrv_SRCS})

macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()
#print_all_variables()
# Link to Cutelyst
target_link_libraries(dmstrsrv
    Cutelyst::Core
    Cutelyst::Session
    Cutelyst::Authentication
    Qt5::Core
    Qt5::Network
    Qt5::Sql
    ${MYSQL_LIBRARY}
)

