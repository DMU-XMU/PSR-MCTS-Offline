
#set(CMAKE_C_COMPILER gcc-mp-4.6)
#set(CMAKE_CXX_COMPILER  g++-mp-4.6)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=gnu++0x -mfpmath=sse -msse -msse2 -msse3 -mtune=native -ftree-vectorize -ffast-math -Wall -gstabs+")

set(CMAKE_SOURCE_DIR "src/")
SET(BASEPATH "${CMAKE_SOURCE_DIR}")
INCLUDE_DIRECTORIES("${BASEPATH}")

PROJECT(brl)
SET(brl_src
	src/envs/simulator.cpp

	src/envs/tiger.cpp
	src/utils/rng.cpp
	src/utils/utils.cpp
	src/utils/utils2.cpp
	src/utils/anyoption.cpp
	src/utils/hr_time.cpp
	
	
	src/planners/mcp/experiment_spectral.cpp
	src/planners/mcp/psrmcts/Matrix.cpp
	
	
	src/planners/mcp/psrmcts/psrmcts.cpp
	src/planners/mcp/psrmcts/symbol.cpp
	src/planners/mcp/psrmcts/spectral.cpp
	
	
	src/planners/mcp/psrmcts/node.cpp
	
	
)

# Note: boost is not essential, dependency could be removed easily
SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "/opt/local/include/")
FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
MESSAGE("${Boost_INCLUDE_DIRS}")
ADD_EXECUTABLE(brl src/main.cpp ${brl_src})

target_link_libraries(brl ${Boost_LIBRARIES}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
)
