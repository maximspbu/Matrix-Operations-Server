cmake_minimum_required(VERSION 3.12)
project(client VERSION 1.0 LANGUAGES C CXX)
set(CMAKE_VERBOSE_MAKEFILE ON)
#find_package (Eigen3 3.3 REQUIRED NO_MODULE)
#link_directories(${Eigen_INCLUDE_DIRS})
find_package(Boost 1.74 REQUIRED)
#link_directories(${Eigen_INCLUDE_DIRS})
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
set(CMAKE_CXX_STANDARD 20)
set(SOURCES source/main.cpp source/client.cpp)
include_directories("include/")
add_executable(client  ${SOURCES})
target_link_libraries(${PROJECT_NAME}
   #${Eigen_LIBRARIES}
   ${Boost_LIBRARIES}
)
