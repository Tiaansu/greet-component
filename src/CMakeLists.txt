file(GLOB_RECURSE 2LOG_FILES
    *.hpp
    *.cpp
)

add_library(${PROJECT_NAME} MODULE ${2LOG_FILES})

GroupSourcesByFolder(${PROJECT_NAME})

if (UNIX)
    set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
target_link_libraries(${PROJECT_NAME} OMP-SDK)

target_link_libraries(${PROJECT_NAME} omp-logger-sdk)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)