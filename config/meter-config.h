// Himanshu

#pragma once

#include <string>
#include <initializer_list>
#include <cstdint>

enum MeterConfigurations : uint8_t
{
    configE_METER_SERIAL_NUMBER,
    configE_METER_MANUFACTURER,
    configE_DEVICE_ID,
    configE_METER_FW_VERSION,
    configE_METER_TYPE,
    configE_METER_CATEGORY,
    configE_CURRENT_RATING,
    configE_YEAR_OF_MANUFACTURE,
    configE_MAX
};

struct MeterConfigType
{
    MeterConfigurations Name;
    std::initializer_list<uint8_t> OID;
    std::string Value;
};

extern std::initializer_list<MeterConfigType> MeterConfig;
