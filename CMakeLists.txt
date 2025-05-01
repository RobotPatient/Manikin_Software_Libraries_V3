cmake_minimum_required(VERSION 3.21)
project(manikin_software_libraries C CXX)

option(MANIKIN_SOFTWARE_BUILD_TESTS "Enable building tests" OFF)

add_library(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/i2c/i2c.c
        ${CMAKE_CURRENT_LIST_DIR}/src/spi/spi.c
        ${CMAKE_CURRENT_LIST_DIR}/src/w25qxx128/w25qxx128.c
        ${CMAKE_CURRENT_LIST_DIR}/src/sample_timer/sample_timer.c
        ${CMAKE_CURRENT_LIST_DIR}/src/vl6180x/vl6180x.c
        ${CMAKE_CURRENT_LIST_DIR}/src/vl53l4cd/vl53l4cd.c
        ${CMAKE_CURRENT_LIST_DIR}/src/sdp810/sdp810.c
        ${CMAKE_CURRENT_LIST_DIR}/src/ads7138/ads7138.c
        ${CMAKE_CURRENT_LIST_DIR}/src/bmm350/bmm350.c
        ${CMAKE_CURRENT_LIST_DIR}/src/bhi360/bhi360.c
        ${CMAKE_CURRENT_LIST_DIR}/src/error_handler/error_handler.c)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CMAKE_C_STANDARD 99)
    set(CMAKE_C_STANDARD_REQUIRED True)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED True)
    set(MANIKIN_SOFTWARE_BUILD_TESTS ON)
endif ()

if (WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive- /WX)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall
            -Wextra
            -Wno-implicit-int-conversion
            -Wpedantic
            -Wshadow
            -Wundef
            -Wconversion
            -Wsign-conversion
            -Wcast-align
            -Wcast-qual
            -Wstrict-prototypes
            -Wdouble-promotion
            -Wformat=2
            -Wformat-security
            -fno-common
            -fstack-protector-strong)
endif ()

if (MANIKIN_SOFTWARE_BUILD_TESTS)
    # target_compile_options(${PROJECT_NAME}) PRIVATE -Werror
    enable_testing()
    add_subdirectory(tests)
else ()

endif ()

# --------------------------------------------------------------------
# Print configuration summary
# --------------------------------------------------------------------
message(STATUS "Configuration Summary:")
message(STATUS "  Project: ${PROJECT_NAME}")
message(STATUS "  Build Tests: ${MANIKIN_SOFTWARE_BUILD_TESTS}")