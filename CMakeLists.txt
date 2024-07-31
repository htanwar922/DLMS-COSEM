cmake_minimum_required(VERSION 3.5.1)      # Himanshu - changed from 3.1 to 3.5.1
if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

# For Raspberry pi
execute_process(COMMAND lsb_release -si OUTPUT_VARIABLE PLATFORM_ID)
message(STATUS "\nPlatform ID : ${PLATFORM_ID}")
if("${PLATFORM_ID}" STREQUAL "Raspbian\n")
  set(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabihf-gcc)
  set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++)
endif()

project(DLMS-COSEM VERSION 1.0.0)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 14)
if(MSVC)
	add_compile_options(/W1)
	find_package(asio REQUIRED)
endif()
find_package(GTest REQUIRED)
find_package(OpenSSL REQUIRED)

#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

#function(dump_cmake_variables)
#    get_cmake_property(_variableNames VARIABLES)
#    list (SORT _variableNames)
#    foreach (_variableName ${_variableNames})
#        if ((NOT DEFINED ARGV0) OR _variableName MATCHES ${ARGV0})
#            message(STATUS "${_variableName}=${${_variableName}}")
#        endif()
#    endforeach()
#endfunction()
#dump_cmake_variables()

set(CMAKE_COLOR_DIAGNOSTICS ON)

function(set_with_path_prefix VAR)
  set(result "")
  foreach(file ${ARGN})
      list(APPEND result ${CMAKE_CURRENT_SOURCE_DIR}/${file})
  endforeach()
  set(${VAR} ${result} CACHE INTERNAL "" FORCE)
endfunction()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(
  ${ASIO_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/config
  ${CMAKE_SOURCE_DIR}/hdlc/include
  ${CMAKE_SOURCE_DIR}/Core/Inc/Linux
  ${CMAKE_SOURCE_DIR}/Core/Inc/lib
  ${CMAKE_SOURCE_DIR}/Core/Inc/lib/DLMS-COSEM
)

add_subdirectory(config)
add_subdirectory(hdlc)
add_subdirectory(Core)

## ==================== Client Definitions ==================== ##

if(NOT "${PLATFORM_ID}" STREQUAL "Raspbian\n")
	add_executable(LinuxClient ${LinuxClient_SOURCES} ${CONFIG_SOURCES} ${HDLC_SOURCES})
	target_compile_definitions(LinuxClient PRIVATE
		ROOT_DIR="${CMAKE_SOURCE_DIR}"
		MODE=CLIENT
		WIN32_LEAN_AND_MEAN
		ASIO_STANDALONE
		NO_DATABASE		# default - off (use database in client mode)
	)

	get_target_property(LinuxClient_DEFINITIONS LinuxClient COMPILE_DEFINITIONS)
	message(STATUS "LinuxClient_DEFINITIONS: ${LinuxClient_DEFINITIONS}")
	target_compile_definitions(DLMS-COSEM-Client PRIVATE ${LinuxClient_DEFINITIONS})

	list(FIND LinuxClient_DEFINITIONS "NO_DATABASE" LinuxClient_DEFINITION_FOUND_INDEX)
	if (LinuxClient_DEFINITION_FOUND_INDEX EQUAL -1)
		target_link_libraries(LinuxClient
			DLMS-COSEM-Client
			${LIBRARIES_FROM_REFERENCES}
			${CMAKE_THREAD_LIBS_INIT}
			mongocxx
			bsoncxx
		)
	else()
		target_link_libraries(LinuxClient
			DLMS-COSEM-Client
			${LIBRARIES_FROM_REFERENCES}
			${CMAKE_THREAD_LIBS_INIT}
			#mongocxx
			#bsoncxx
		)
	endif()
endif()

## ==================== Server Definitions ==================== ##

if(TRUE)
	add_executable(LinuxServer ${LinuxServer_SOURCES} ${CONFIG_SOURCES} ${HDLC_SOURCES})
	target_compile_definitions(LinuxServer PRIVATE
		ROOT_DIR="${CMAKE_SOURCE_DIR}"
		MODE=SERVER
		WIN32_LEAN_AND_MEAN
		ASIO_STANDALONE
		NO_DATABASE
		# MODBUS			# default - on (use modbus in server mode)
	)
	get_target_property(LinuxServer_DEFINITIONS LinuxServer COMPILE_DEFINITIONS)
	message(STATUS "LinuxServer_DEFINITIONS: ${LinuxServer_DEFINITIONS}")
	target_compile_definitions(DLMS-COSEM-Server PRIVATE ${LinuxServer_DEFINITIONS})

	list(FIND LinuxServer_DEFINITIONS MODBUS LinuxServer_DEFINITION_FOUND_INDEX)
	if (LinuxServer_DEFINITION_FOUND_INDEX EQUAL -1)
		target_link_libraries(LinuxServer
			DLMS-COSEM-Server
			${LIBRARIES_FROM_REFERENCES}
			${CMAKE_THREAD_LIBS_INIT}
			# modbus
		)
	else()
		target_link_libraries(LinuxServer
			DLMS-COSEM-Server
			${LIBRARIES_FROM_REFERENCES}
			${CMAKE_THREAD_LIBS_INIT}
			modbus
		)
	endif()
endif()

## ==================== Google Tests ==================== ##

add_subdirectory(test)
