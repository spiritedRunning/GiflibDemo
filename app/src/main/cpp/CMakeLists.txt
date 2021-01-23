cmake_minimum_required(VERSION 3.4.1)

file(GLOB libgif_src ${CMAKE_SOURCE_DIR}/giflib/*.c)

include_directories(${CMAKE_SOURCE_DIR}/giflib)

add_library(
             native-lib
             SHARED
             ${libgif_src}
             JavaInputStream.cpp
             GifFrame.cpp
             native-lib.cpp )


target_link_libraries(
                       native-lib
                        jnigraphics
                        android
                       log)