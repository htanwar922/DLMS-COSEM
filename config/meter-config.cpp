// Himanshu

#include "DLMSVector.h"
#include "DLMSValue.h"

#include "meter-config.h"

std::initializer_list <MeterDataConfigType> MeterDataConfig = {
    {   configE_INVOCATION_COUNTER,     {0, 0, 43,  1,  3, 255},    0                   },
    {   configE_METER_SERIAL_NUMBER,    {0, 0, 96,  1,  0, 255},    "0000000000000000"  },
    {   configE_METER_MANUFACTURER,     {0, 0, 96,  1,  1, 255},    "DIN"               },
    {   configE_DEVICE_ID,              {0, 0, 96,  1,  2, 255},    "abcxyz1234"        },
    {   configE_METER_FW_VERSION,       {1, 0,  0,  2,  0, 255},    "1.0"               },
    {   configE_METER_TYPE,             {0, 0, 94, 91,  9, 255},    "Single Phase"      },
    {   configE_METER_CATEGORY,         {0, 0, 94, 91, 11, 255},    "Residential"       },
    {   configE_CURRENT_RATING,         {0, 0, 94, 91, 12, 255},    "5A"                },
    {   configE_YEAR_OF_MANUFACTURE,    {0, 0, 96,  1,  4, 255},    "2021"              },
};

std::initializer_list <MeterRegisterConfigType> MeterRegisterConfig = {
    {  configE_CONST_ACTIVE_ENERGY,         {1, 0,  0,  7, 0, 255 },  100.0,    3,    0  },
    {  configE_AVG_VOLTAGE,                 {1, 0, 12, 27, 0, 255 },  230.0,    0,    0  },
    {  configE_BLOCK_ENERGY_KWH_IMPORT,     {1, 0,  1, 29, 0, 255 },  100.0,    3,    0  },
    {  configE_BLOCK_ENERGY_KVAH_IMPORT,    {1, 0,  9, 29, 0, 255 },  150.0,    3,    0  },
    {  configE_BLOCK_ENERGY_KWH_EXPORT,     {1, 0,  2, 29, 0, 255 },   99.9,    3,    0  },
    {  configE_BLOCK_ENERGY_KVAH_EXPORT,    {1, 0, 10, 29, 0, 255 },  149.9,    3,    0  },
};

std::initializer_list<AssociationConfigType> AssociationConfigPC = {
    { 15, { 0, 0, 40, 0, 0, 255 } },
    { 15, { 0, 0, 40, 0, 1, 255 } },
    { 15, { 0, 0, 40, 0, 2, 255 } },
    { 15, { 0, 0, 40, 0, 3, 255 } },
    //{ 15, { 0, 0, 40, 0, 4, 255 } },
    //{ 15, { 0, 0, 40, 0, 5, 255 } },
    //{ 15, { 0, 0, 40, 0, 6, 255 } },

    { 1, { 0, 0, 96, 1, 0, 255 } },
    { 1, { 0, 0, 96, 1, 1, 255 } },
    { 1, { 0, 0, 96, 1, 2, 255 } },
    { 1, { 1, 0, 0, 2, 0, 255 } },
    { 1, { 0, 0, 94, 91, 9, 255 } },
    { 1, { 0, 0, 94, 91, 11, 255 } },
    { 1, { 0, 0, 94, 91, 12, 255 } },
    { 1, { 0, 0, 96, 1, 4, 255 } },
    { 1, { 0, 0, 43, 1, 3, 255 } }
};

std::initializer_list<AssociationConfigType> AssociationConfigMR = {
    { 3, { 1, 0, 0, 7, 0, 255 } },
};

std::initializer_list<AssociationConfigType> AssociationConfigUS = {
    { 15, { 0, 0, 40, 0, 0, 255 } },
    { 15, { 0, 0, 40, 0, 3, 255 } },
    { 3, { 1, 0, 0, 7, 0, 255 } },
    { 7, { 0, 0, 94, 91, 10, 255 } },
};

std::initializer_list<AssociationConfigType> AssociationConfigPUSH = {};
std::initializer_list<AssociationConfigType> AssociationConfigFOTA = {};
std::initializer_list<AssociationConfigType> AssociationConfigIHD = {};
