cmake_minimum_required(VERSION 3.21)
project(manikin_software_libraries C CXX)

option(MANIKIN_SOFTWARE_BUILD_TESTS "Enable building tests" OFF)

add_library(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/i2c/i2c.c
        ${CMAKE_CURRENT_LIST_DIR}/src/spi/spi.c
        ${CMAKE_CURRENT_LIST_DIR}/src/sample_timer/sample_timer.c
        ${CMAKE_CURRENT_LIST_DIR}/src/vl6180x/vl6180x.c
        ${CMAKE_CURRENT_LIST_DIR}/src/error_handler/error_handler.c
        ${CMAKE_CURRENT_LIST_DIR}/src/sdp810/sdp810.c
        ${CMAKE_CURRENT_LIST_DIR}/src/ads7138/ads7138.c
        ${CMAKE_CURRENT_LIST_DIR}/src/bmm350/bmm350.c
        ${CMAKE_CURRENT_LIST_DIR}/src/bhi360/bhi360.c)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_STANDARD_REQUIRED True)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED True)
    set(MANIKIN_SOFTWARE_BUILD_TESTS ON)
endif ()


if (MANIKIN_SOFTWARE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
else ()

endif ()
