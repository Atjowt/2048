cmake_minimum_required(VERSION 3.10..3.50)
project(2048 LANGUAGES C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_GENERATE_COMPILE_COMMANDS ON)
add_executable(2048 lib/libraylib.a src/main.c)
target_include_directories(2048 PRIVATE include)
target_link_directories(2048 PRIVATE lib)
target_link_libraries(2048 PRIVATE m raylib)
add_custom_command (
    TARGET 2048 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:2048>/assets
)
