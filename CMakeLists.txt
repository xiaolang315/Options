CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT("options")

set(ENABLE_TEST OFF CACHE BOOL "Enable the test")

MACRO(sort_files source_files)
  SET(sgbd_cur_dir ${CMAKE_CURRENT_SOURCE_DIR})
  FOREACH(sgbd_file ${${source_files}})
    STRING(REGEX REPLACE ${sgbd_cur_dir}/\(.*\) \\1 sgbd_fpath ${sgbd_file})
    STRING(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
    STRING(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
    IF(MSVC)
      string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
    ENDIF(MSVC)
    IF(sgbd_nogroup)
      SET(sgbd_group_name "\\")
    ENDIF(sgbd_nogroup)
    SOURCE_GROUP(${sgbd_group_name} FILES ${sgbd_file})
  ENDFOREACH(sgbd_file)
ENDMACRO(sort_files)

INCLUDE_DIRECTORIES( 
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/test"
)

IF(MSVC)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS )
  ADD_DEFINITIONS(-DMSVC_VMG_ENABLED)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /vmg")
ENDIF(MSVC)

IF(UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
ENDIF(UNIX)

set(OPTIONS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(DIRECTORY include DESTINATION include)

add_subdirectory(src)

if(ENABLE_TEST)
    add_subdirectory(test)
endif()

