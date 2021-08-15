CMAKE_MINIMUM_REQUIRED(VERSION 3.1.4)
set(PROJECT_NAME QpidSymbols)
project(${PROJECT_NAME})
set(CMAKE_BUILD_TYPE "Debug")
file(GLOB QpidSymbols_SRCS ./*.cpp)

add_executable(${PROJECT_NAME} ${QpidSymbols_SRCS})
