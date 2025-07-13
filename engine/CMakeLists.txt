set(TARGET_NAME engine)

set(INCLUDES ${CMAKE_CURRENT_LIST_DIR}/include
             ${PROJECT_COMMON_LIB_PATH}/include)
set(DEPENDENCIES common-lib)
set(LINKS ${CMAKE_CURRENT_BINARY_DIR}
          ${CMAKE_CURRENT_BINARY_DIR}/Debug
          ${CMAKE_CURRENT_BINARY_DIR}/Release)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

add_library(${TARGET_NAME} SHARED ${SOURCES})

target_include_directories(${TARGET_NAME} 
                           PRIVATE ${INCLUDES})

target_link_directories(${TARGET_NAME}
                        PRIVATE ${LINKS})

target_link_libraries(${TARGET_NAME}
                      PRIVATE ${DEPENDENCIES})

add_subdirectory(test)
