cmake_minimum_required(VERSION 2.6)
project(mediacut)

set(CMAKE_CXX_FLAGS "-std=c++11")

include_directories("/home/acmax/app/ffmpeg-dev/include")

link_directories("/home/acmax/app/ffmpeg-dev/lib")

add_executable(mediacut main.cpp)

target_link_libraries(mediacut avformat avcodec avutil pthread)
#install(TARGETS mediacut RUNTIME DESTINATION bin)
