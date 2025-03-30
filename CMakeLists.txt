cmake_minimum_required(VERSION 3.0)
project(chat)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)
#执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#头文件位置
include_directories(${PROJECT_SOURCE_DIR}/inc)
include_directories(${PROJECT_SOURCE_DIR}/inc/server)
include_directories(${PROJECT_SOURCE_DIR}/inc/server/db)
include_directories(${PROJECT_SOURCE_DIR}/inc/server/redis)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)

#子目录
add_subdirectory(src)