cmake_minimum_required(VERSION 3.10)

set(MAINTGT TeknicMotor) # set the project name

#TODO: Make this handle debug vs release
set(CMAKE_BUILD_TYPE "Debug") 

# VCpkg setup for WIN
# set(VCPKG_TARGET_ARCHITECTURE x64) # 
# set(VCPKG_CRT_LINKAGE dynamic)     # Cpp Runtime
# set(VCPKG_LIBRARY_LINKAGE dynamic) # 


# set the project name
project(
    ${MAINTGT}
    VERSION 0.1
    DESCRIPTION "Wrapper over Teknic Clearview/Clearpath motors"
    HOMEPAGE_URL "https://bensmaialab.org/"
    LANGUAGES CXX
)
# Boost support based on: https://github.com/iso8859/vcpkg-boost
if(WIN32)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake_msvc.cmake)
    configure_msvc_runtime("${VCPKG_TARGET_TRIPLET}")
endif()

# C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message("-- Linking Boost libraries")
find_package(Boost REQUIRED COMPONENTS log)

# Teknic sFoundation20 DLL
if(WIN32)
    add_subdirectory(sfoundation)
endif()
get_cmake_property(sfoundationDir SFOUNDATION_DIR ) # Hackish to work around scope
include_directories(
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/src
    ${sfoundationDir}/inc
    ${Boost_INCLUDE_DIRS}
)

# After all the includes so it's aware. Right.
add_subdirectory(src)

# add the executable
add_executable(${MAINTGT} "${PROJECT_SOURCE_DIR}/src/main.cpp")

target_link_libraries(${MAINTGT}
    debug "${sfoundationDir}/lib/debug/sFoundation20.lib"
    # optimized ${Boost_LIBRARY_DIR_RELEASE}/boost_system-vc140-mt.lib
    debug "${Boost_LIBRARY_DIR_DEBUG}/boost_system-vc140-mt-gd.lib"
    debug "${Boost_LIBRARY_DIR_DEBUG}/boost_log-vc140-mt-gd.lib"
    wrapNode
    motorAPI
)

# install(TARGETS ${MAINTGT} DESTINATION bin)


if(WIN32)
    add_custom_command(TARGET ${MAINTGT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${sfoundationDir}/lib/debug/sFoundation20.dll $<TARGET_FILE_DIR:${MAINTGT}>)
endif()

# To dump all vars for debugging:
# dump_all()
