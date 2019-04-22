cmake_minimum_required(VERSION 3.0)
project(GLAD VERSION 0.1.29 LANGUAGES C)
add_library(GLAD STATIC src/glad.c)
target_include_directories(GLAD PUBLIC include)