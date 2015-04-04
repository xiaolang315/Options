project(options_test)

include_directories(${OPTIONS_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../test)

FILE(GLOB_RECURSE all_files
*.cpp
*.cc
*.c++
*.c
*.C)

sort_files(all_files)

add_executable(options_test ${all_files})

target_link_libraries(options_test options infra hamcrest magellan stdc++)

