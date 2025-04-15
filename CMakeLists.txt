cmake_minimum_required(VERSION 3.21)
project(manikin_software_libraries C CXX)

option(MANIKIN_SOFTWARE_BUILD_TESTS "Enable building tests" YES)

add_library(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/i2c/i2c.c
                            ${CMAKE_CURRENT_LIST_DIR}/src/sample_timer/sample_timer.c
                            ${CMAKE_CURRENT_LIST_DIR}/src/vl6180x/vl6180x.c)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(CMAKE_C_STANDARD 11)
  set(CMAKE_C_STANDARD_REQUIRED True)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED True)
  set(MANIKIN_SOFTWARE_BUILD_TESTS ON)
endif()


if(MANIKIN_SOFTWARE_BUILD_TESTS)
  add_subdirectory(tests)
else()

endif()
