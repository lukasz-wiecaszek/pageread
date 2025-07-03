cmake_minimum_required(VERSION 3.14) # it's just a nice number

project(pageread VERSION 0.0.1)

# sets various paths used in e.g. pc.in files as well as install target
include(GNUInstallDirs)

message(STATUS "Processing CMakeLists.txt for: " ${PROJECT_NAME} " " ${PROJECT_VERSION})

# if you are building in-source, this is the same as CMAKE_SOURCE_DIR, otherwise
# this is the top level directory of your build tree
message(STATUS "CMAKE_BINARY_DIR:         " ${CMAKE_BINARY_DIR})

# if you are building in-source, this is the same as CMAKE_CURRENT_SOURCE_DIR, otherwise this
# is the directory where the compiled or generated files from the current CMakeLists.txt will go to
message(STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR})

# this is the directory, from which cmake was started, i.e. the top level source directory
message(STATUS "CMAKE_SOURCE_DIR:         " ${CMAKE_SOURCE_DIR})

# this is the directory where the currently processed CMakeLists.txt is located in
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_C_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
         "Choose the type of build. Options are: {Release, Debug}." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

message(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})

add_compile_options(-Wall -Werror -pedantic)

if(CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug$")
    add_compile_options(-ggdb3)
else()
    add_compile_options(-O2)
endif()

configure_file(version.h.in version.h)

#------------------------------------------------------------------------------
#                             TARGET(S) DEFINITION(S)
#------------------------------------------------------------------------------
set(PAGEREAD_SRCS
    pageread.c
)

add_executable(${PROJECT_NAME}
    ${PAGEREAD_SRCS}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR} # this is the directory where 'version.h' will be configured
)

#------------------------------------------------------------------------------
#                                 INSTALLATION
#------------------------------------------------------------------------------
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Add all targets to the install-tree export set
install(EXPORT ${PROJECT_NAME}-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Add all targets to the build-tree export set
export(EXPORT ${PROJECT_NAME}-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}-targets.cmake
)
