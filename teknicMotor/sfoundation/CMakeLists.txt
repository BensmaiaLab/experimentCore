# Sets up Teknic's Clearpath Motor library
# 

if(WIN32)
    message("-- Linking TeknicMotor SFoundation20.dll")
    # debug lib version
    add_library(sFoundation20d IMPORTED SHARED sFoundation20.dll)
    set_target_properties(sFoundation20d PROPERTIES
        LINKER_LANGUAGE C
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/inc"
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/debug"
    )
    message("-- Setting SFOUNDATION_DIR to ${CMAKE_CURRENT_SOURCE_DIR}")
    # set(SFOUNDATION_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
    set_property(GLOBAL PROPERTY SFOUNDATION_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()
