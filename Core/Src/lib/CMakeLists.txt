# For Raspberry pi
if(NOT "${PLATFORM_ID}" STREQUAL "Raspbian\n")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG -DASIO_STANDALONE -DASIO_HAS_STD_CHRONO")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_DEBUG -DASIO_STANDALONE -DASIO_HAS_STD_CHRONO")
endif()

add_subdirectory(DLMS-COSEM)
# add_subdirectory(googletest-release-1.7.0)	# Himanshu: Commented out for now