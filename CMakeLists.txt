cmake_minimum_required(VERSION 3.13)

# Accept SDK path from either CMake cache variable or environment variable.
if(NOT DEFINED PICO_SDK_PATH)
    if(DEFINED ENV{PICO_SDK_PATH})
        set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
    else()
        message(FATAL_ERROR "PICO_SDK_PATH is not set. Set env var or pass -DPICO_SDK_PATH=<path>.")
    endif()
endif()

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico_irrigation_system C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/src/*.c
    ${CMAKE_CURRENT_LIST_DIR}/src/*.h
)

set(PROJECT_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/src)
foreach(project_source IN LISTS PROJECT_SOURCES)
    get_filename_component(project_source_dir ${project_source} DIRECTORY)
    list(APPEND PROJECT_INCLUDE_DIRS ${project_source_dir})
endforeach()
list(REMOVE_DUPLICATES PROJECT_INCLUDE_DIRS)

# Initialize the SDK after project() is declared.
pico_sdk_init()

add_executable(pico_irrigation_system
    ${PROJECT_SOURCES}
)

target_include_directories(pico_irrigation_system PRIVATE
    ${PROJECT_INCLUDE_DIRS}
)

# Enable USB stdio and disable UART stdio for convenient serial logging.
pico_enable_stdio_usb(pico_irrigation_system 1)
pico_enable_stdio_uart(pico_irrigation_system 0)

target_link_libraries(pico_irrigation_system
    pico_stdlib
)

pico_add_extra_outputs(pico_irrigation_system)
