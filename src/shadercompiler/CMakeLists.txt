cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME sge_shadercompiler)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

sge_add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER sge/shadercompiler)
target_link_libraries(${PROJECT_NAME} PRIVATE sge_render)