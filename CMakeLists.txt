cmake_minimum_required( VERSION 3.2 )

project( nidaq )

SET( CMAKE_CXX_STANDARD 11)
SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -std=c++11 -lpthread -ldl" )
SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-D_POSIX_C_SOURCE=200809L -L/usr/lib/x86_64-linux-gnu -lm -lnidaqmx" )

find_package( HDF5 CONFIG REQUIRED )
find_package( polaris CONFIG REQUIRED )

include_directories( include h5manager/include )
file( GLOB SOURCES "src/*.cpp" "h5manager/src/*.cpp" )

add_library( nidaq SHARED ${SOURCES} )
install( TARGETS nidaq )

