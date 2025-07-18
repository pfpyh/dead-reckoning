set(TARGET_NAME test-${TARGET_NAME})
project(${TARGET_NAME})

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

set(INCLUDES ${CMAKE_CURRENT_LIST_DIR}/include
             ${GTEST_INCLUDES}
             ${INCLUDES})

set(LINKS ${GTEST_LINKS}
          ${LINKS})

set(DEPENDENCIES ${DEPENDENCIES}
                 engine
                 gtest
                 gmock)

add_executable(${TARGET_NAME} ${SOURCES})

target_include_directories(${TARGET_NAME}
                           PRIVATE 
                           ${INCLUDES})

target_link_directories(${TARGET_NAME}
                        PRIVATE 
                        ${LINKS})

target_link_libraries(${TARGET_NAME}
                      PRIVATE 
                      ${DEPENDENCIES})

include(GoogleTest)
gtest_discover_tests(${TARGET_NAME})
