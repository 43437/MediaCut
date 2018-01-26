cmake_minimum_required(VERSION 2.6)
project(mediacuts)

set(base_dir ${CMAKE_CURRENT_LIST_DIR})

set(src_dir ${base_dir})

include_directories("/home/acmax/app/ffmpeg-dev/include")

link_directories("/home/acmax/app/ffmpeg-dev/lib")

set(CMAKE_CXX_FLAGS "-std=c++11")

file(GLOB src_files ${src_dir}/*.cpp)

add_executable(mediacuts ${src_files})

target_link_libraries(mediacuts avformat avcodec avutil)

#install(TARGETS mediacuts RUNTIME DESTINATION bin)
