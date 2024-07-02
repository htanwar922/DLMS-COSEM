
include_directories(
  ${CMAKE_SOURCE_DIR}/config
  ${CMAKE_SOURCE_DIR}/hdlc/include
  ${CMAKE_SOURCE_DIR}/Core/Inc/Linux
  ${CMAKE_SOURCE_DIR}/Core/Inc/lib
  ${CMAKE_SOURCE_DIR}/Core/Inc/lib/DLMS-COSEM
  ${CMAKE_CURRENT_SOURCE_DIR}/
)

add_executable(DLMS-COSEM-TEST
	DLMS-COSEM-TEST.cpp
	UnitTest-APDU-AARE.cpp
	UnitTest-APDU-AARQ.cpp
	UnitTest-APDU-ACTION-Request.cpp
	UnitTest-APDU-ACTION-Response.cpp
	UnitTest-APDU-GET-Request.cpp
	UnitTest-APDU-GET-Response.cpp
	UnitTest-APDU-RLRE.cpp
	UnitTest-APDU-RLRQ.cpp
	UnitTest-APDU-xDLMS.cpp
	UnitTest-COSEM-Client.cpp
	UnitTest-COSEMObjectInstanceID.cpp
	UnitTest-DLMSValue.cpp
	UnitTest-HDLC-Address.cpp
	UnitTest-HDLC-Client.cpp
	UnitTest-HDLC-LLC.cpp
	UnitTest-HDLC-MAC.cpp
	UnitTest-HDLC-Packet.cpp
	UnitTest-HDLC-Server.cpp
	UnitTest-ICOSEMInterface.cpp
	UnitTest-Interfaces-COSEMData.cpp
	UnitTest-Memory.cpp
	UnitTest-StateMachine.cpp
)

if(NOT "${PLATFORM_ID}" STREQUAL "Raspbian\n")
	target_link_libraries(DLMS-COSEM-TEST DLMS-COSEM-Client ${GTEST_LIBRARIES} "${CMAKE_THREAD_LIBS_INIT}")
else()
	target_link_libraries(DLMS-COSEM-TEST DLMS-COSEM-Server ${GTEST_LIBRARIES} "${CMAKE_THREAD_LIBS_INIT}")
endif()