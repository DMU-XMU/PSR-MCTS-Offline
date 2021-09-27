cmake_minimum_required(VERSION 3.5)
project(Q-Learning)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        main.cpp
        utils/environment.cpp
        utils/environment.h
        utils/utils2.cpp
        utils/utils2.h
        utils/symbol.cpp
        utils/symbol.h
        utils/simulator.cpp
        utils/simulator.h
        utils/coord.cpp
        utils/coord.h
        utils/grid.h
        )

SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "/opt/local/include/")
FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
MESSAGE("${Boost_INCLUDE_DIRS}")

add_executable(Q-Learning ${SOURCE_FILES})

target_link_libraries(Q-Learning ${Boost_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        )