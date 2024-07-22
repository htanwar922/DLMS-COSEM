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
};

std::initializer_list<AssociationConfigType> AssociationConfigMR = {
    { 3, { 1, 0, 0, 7, 0, 255} },
};
std::initializer_list<AssociationConfigType> AssociationConfigUS = {};
std::initializer_list<AssociationConfigType> AssociationConfigPUSH = {};
std::initializer_list<AssociationConfigType> AssociationConfigFOTA = {};
std::initializer_list<AssociationConfigType> AssociationConfigIHD = {};
