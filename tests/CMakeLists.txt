cmake_minimum_required(VERSION 3.10)

project(${PROJECT_NAME}_tests)

find_package(GTest REQUIRED)

add_executable(
  ${PROJECT_NAME}
  test_dummy.cpp
)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE
  custom_allocator_lib
  GTest::gtest
  GTest::gtest_main
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
