include(FetchContent)
FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.8.0
)

# CMake 3.14+
FetchContent_MakeAvailable(catch2)

include(Catch)

FetchContent_Declare(
        fff
        GIT_REPOSITORY https://github.com/meekrosoft/fff.git
        GIT_TAG master
)

FetchContent_MakeAvailable(fff)

add_library(hal_mock ${CMAKE_CURRENT_LIST_DIR}/test_stubs/mocks/fake_i2c_functions.c
        ${CMAKE_CURRENT_LIST_DIR}/test_stubs/mocks/fake_timer_functions.c)
target_include_directories(hal_mock PUBLIC test_stubs/mocks
                                           test_stubs/conf)
target_link_libraries(hal_mock fff)
target_compile_definitions(hal_mock PUBLIC "FFF_GENERATE")

target_link_libraries(${PROJECT_NAME} hal_mock)

add_executable(test_i2c ${CMAKE_CURRENT_LIST_DIR}/i2c/i2c_test.cpp)
target_link_libraries(test_i2c ${PROJECT_NAME} Catch2 fff hal_mock)
catch_discover_tests(test_i2c)

add_executable(test_timer ${CMAKE_CURRENT_LIST_DIR}/timer/timer_test.cpp)

target_link_libraries(test_timer ${PROJECT_NAME} Catch2 fff)
catch_discover_tests(test_timer)
