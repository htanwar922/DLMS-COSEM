// Himanshu

#pragma once

#include <string>
#include <vector>

#define CLIENT 0
#define SERVER 1

#if __cplusplus < 201402L
#error "C++14 or later required"
#endif

/*
The MODE and ROOT_DIR are automatically defined in the ${workspaceFolder}/CMakelists.txt file.
Additionally, in ${workspaceFolder}/CMakelists.txt, modify the following lines as required:
    Under Server Definitions section:
		MODBUS			# default - on (use modbus in server mode)
    Under Client Definitions section
		NO_DATABASE		# default - off (use database in client mode)
*/
#ifndef MODE
#define MODE SERVER
#endif

#ifndef ROOT_DIR
#define ROOT_DIR "../"									// if running from build directory
#endif

extern const uint8_t AvailableRegisterOBISGroupC[27];
extern const uint16_t MeterRegisterAddress[27];
extern const char * RegisterParamName[27];
extern std::vector<uint8_t> SelectedRegistersOBISGroupC;

#define n_PARAMS (SelectedRegistersOBISGroupC.size())	// No. of parameters selected initially
#define n_METERS 2										// No. of meters registered initially - not used currently.

int FindMeterRegisterByOBISGroupC(uint8_t OBISGroupC);
const char * FindRegisterParamNameByOBISGroupC(uint8_t OBISGroupC);
float GetRegisterDefaultValue(int reg);

#define LOG_LVL_ALL                                 0           // Lowest priority
#define LOG_LVL_DEBUG                               1
#define LOG_LVL_INFO                                2
#define LOG_LVL_NOTICE                              3
#define LOG_LVL_WARNING                             4
#define LOG_LVL_ALERT                               5
#define LOG_LVL_ERROR                               6
#define LOG_LVL_CRITICAL                            7
#define LOG_LVL_EMERGENCY                           8
#define LOG_LVL_ALWAYS                              9           // Highest priority

#define configM_LOG_LVL								LOG_LVL_DEBUG		// Change log level here

#include "log.h"

#ifdef _MSC_VER
#include "msvc-config.h"
#else
#include "gcc-config.h"
#endif // _MSC_VER

#include "meter-config.h"

const std::initializer_list<uint8_t> GuruxCLIClientSystemTitle = { 0x4d, 0x4d, 0x4d, 0x00, 0x00, 0xbc, 0x61, 0x4e };
#define GLO_ACCESS_REQUEST_TAG 0x00
#define GLO_ACCESS_RESPONSE_TAG 0x00
