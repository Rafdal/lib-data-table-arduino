cmake_minimum_required(VERSION 3.0.0)
project(datatable_test VERSION 0.1.0)

include(CTest)
enable_testing()

add_compile_definitions(CPP_TESTING=1)
add_executable(datatable_test main.cpp ../src/DataTable.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
