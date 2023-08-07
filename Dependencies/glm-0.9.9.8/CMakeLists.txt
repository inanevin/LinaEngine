cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)

set(GLM_VERSION "0.9.9")
project(glm VERSION ${GLM_VERSION} LANGUAGES CXX)

add_subdirectory(glm)
add_library(glm::glm ALIAS glm)
