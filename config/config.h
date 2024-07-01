// Himanshu

#pragma once

#include <string>
#include <vector>

#define CLIENT 0
#define SERVER 1

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

extern const char * addresses[n_METERS];
extern const std::string packet_types[2];

#define BATCH_SIZE									30	// sampling times
#define SAMPLING_INTERVAL 							0.5	// seconds
#define POLLING_INTERVAL							30	// seconds

#if defined(STM32F2) or defined(STM32F4)
#define STM32
#endif

#ifdef STM32
#include "cmsis_os.h"
#endif

#ifndef STM32
#define MODBUS_UART_PATH							"/dev/ttyUSB0"	// change to "" or "/dev/ttyUSB1" if not needed
#define MODBUS_BAUD 9600
#endif

#ifndef STM32
#define configM_USE_DATABASE                        0
#define MONGODB_URI									"mongodb://localhost:27017"
#define MONGODB_DATABASE_NAME						"SmartMeters"
#define MONGODB_COLLECTION_NAME						"collection"
#define BUFFER_LEN									(40*1024)
#define CSVFILENAME(suffix)							("../../../data/received_data_meter-" suffix ".csv")
#endif

#define configM_DUMMY_OBIS_CODE						"0-0:96.1.0*255"
#define configM_CONTINUOUS_OBIS_CODE_ORIGINAL		"1-0:254.7.0*255"
#define configM_CONTINUOUS_OBIS_CODE_COMPRESSED		"1-0:255.7.0*255"
#define configM_CONTINUOUS_VG_ORIGINAL				128U		// OBIS Code param for continuous polling
#define configM_CONTINUOUS_VG_COMPRESSED 			128U		// OBIS Code param for continuous polling

#define configM_RUN_DLMS_SERVER_THREAD				1
#define configM_STM32_SERIAL_SOCKET					0
#define configM_STM32_WIFI_SOCKET					1

#define configM_WIFI_THREAD							1

#define configM_METER_THREAD						1			// Modules (Functors)
#define configM_REAL_MODBUS_DEVICE					0			// Device (HW)
#define configM_USE_CSV_FILE_DATASET                1           // File (Data)
#define configM_DATASET_CSV_FILENAME                ROOT_DIR "/data/300_1_original_data.csv"
#define configM_OPTIMISE_CRC_FOR_SIZE				0			// [STM32 only] Caution: Uses a time-complex code
#define configM_USE_SD_CARD							0           // [STM32 only]

#define configM_ENABLE_COMPRESSED_DATA              1			// Compressed Data
#ifndef STM32
#define configM_ENABLE_ORIGINAL_DATA				1			// Original Data [Linux]
#else
#define configM_ENABLE_ORIGINAL_DATA				0			// Original Data [STM32]
#endif

#define configM_DEBUGGER_THREAD						1
#define configM_DEBUGGER_TIMESTAMPS					0

#if defined(STM32) and configM_USE_SD_CARD
#define configM_SD_DRIVE_MOUNT_PATH					""		    // Must follow this format
#define configM_FILE_SAVED_DATA_FORMAT				(configM_SD_DRIVE_MOUNT_PATH "%05u.CSV")
#define configM_MAX_SAVED_BATCH_FILES				10
#endif

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
