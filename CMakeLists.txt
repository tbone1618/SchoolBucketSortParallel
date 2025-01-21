# Set minimum CMAKE
cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project( BucketSort )

# require C++20
set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_EXTENSIONS OFF )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

set( APP_EXECUTABLE BucketSortTest )

if(NOT CMAKE_BUILD_TYPE)
 set(CMAKE_BUILD_TYPE Release)
endif()

include_directories( inc )

find_package( Threads REQUIRED )

ADD_EXECUTABLE( ${APP_EXECUTABLE} "src/bucketsort-test.cpp")
TARGET_LINK_LIBRARIES( ${APP_EXECUTABLE} LINK_PUBLIC ${LIB_NAME} Threads::Threads )

set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT ${APP_EXECUTABLE} )

include (CTest)
add_test(${APP_EXECUTABLE}_testSmallSingleThreadedSorts ${APP_EXECUTABLE} 1)
add_test(${APP_EXECUTABLE}_testLargeSingleThreadedSort ${APP_EXECUTABLE} 2)
add_test(${APP_EXECUTABLE}_testSmallMultiThreadedSorts ${APP_EXECUTABLE} 3)
add_test(${APP_EXECUTABLE}_testAll ${APP_EXECUTABLE} 4)

find_program(VALGRIND "valgrind")
if(VALGRIND)
	add_custom_target(valgrind
		COMMAND "${VALGRIND}" --tool=memcheck --leak-check=yes --show-reachable=yes --error-exitcode=1 $<TARGET_FILE:${APP_EXECUTABLE}> valgrind_mode)
endif()
