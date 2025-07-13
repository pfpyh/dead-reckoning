cmake_minimum_required(VERSION 3.16)

project(dead-reckoning)

set(CMAKE_CXX_STANDARD 17)
set(PROJECT_APPLICATION_PATH ${CMAKE_CURRENT_LIST_DIR}/application)
set(PROJECT_ENGINE_PATH ${CMAKE_CURRENT_LIST_DIR}/engine)
set(PROJECT_LIBS_PATH ${CMAKE_CURRENT_LIST_DIR}/libs)

set(PROJECT_COMMON_LIB_PATH ${PROJECT_LIBS_PATH}/common-lib)

# GoogleTest ############################################
set(GTEST_ROOT ${PROJECT_COMMON_LIB_PATH}/libs/googletest)
set(GTEST_INCLUDES ${GTEST_ROOT}/googletest/include
                   ${GTEST_ROOT}/googlemock/include)
set(GTEST_LINKS ${CMAKE_CURRENT_BINARY_DIR}/lib)

message(STATUS GTEST_ROOT=${GTEST_ROOT})
message(STATUS GTEST_INCLUDES=${GTEST_INCLUDES})
message(STATUS GTEST_LINKS=${GTEST_LINKS})
##########################################################

if (WIN32)
    add_definitions(-DWINDOWS)
elseif (UNIX)
    add_definitions(-DLINUX)
endif()

enable_testing()

# add_subdirectory(application)
add_subdirectory(engine)
add_subdirectory(libs)
