// Himanshu

#include "meter-config.h"

std::initializer_list <MeterConfigType> MeterConfig = {
    {   configE_METER_SERIAL_NUMBER,    {0, 0, 96, 1, 0, 255},      "0000000000000000"     },
    {   configE_METER_MANUFACTURER,     {0, 0, 96, 1, 1, 255},      "DIN"                  },
    {   configE_DEVICE_ID,              {0, 0, 96, 1, 2, 255},      "abcxyz1234"           },
    {   configE_METER_FW_VERSION,       {1, 0, 0, 2, 0, 255},       "1.0"                  },
    {   configE_METER_TYPE,             {0, 0, 94, 91, 9, 255},     "Single Phase"         },
    {   configE_METER_CATEGORY,         {0, 0, 94, 91, 11, 255},    "Residential"          },
    {   configE_CURRENT_RATING,         {0, 0, 94, 91, 12, 255},    "5A"                   },
    {   configE_YEAR_OF_MANUFACTURE,    {0, 0, 96, 1, 4, 255},      "2021"                 },
};