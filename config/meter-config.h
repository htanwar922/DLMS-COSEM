// Himanshu

#pragma once

#include <string>
#include <initializer_list>
#include <vector>
#include <cstdint>

#include "DLMSValue.h"

enum MeterDataConfigurations : uint8_t
{
    configE_INVOCATION_COUNTER,
    configE_METER_SERIAL_NUMBER,
    configE_METER_MANUFACTURER,
    configE_DEVICE_ID,
    configE_METER_FW_VERSION,
    configE_METER_TYPE,
    configE_METER_CATEGORY,
    configE_CURRENT_RATING,
    configE_YEAR_OF_MANUFACTURE,
    configE_DATA_MAX
};

struct MeterDataConfigType
{
    MeterDataConfigurations Name;
    std::initializer_list<uint8_t> OID;
    EPRI::DLMSValue Value;
};

enum MeterRegisterConfigurations : uint8_t
{
    configE_CONST_ACTIVE_ENERGY,
    configE_AVG_VOLTAGE,
    configE_BLOCK_ENERGY_KWH_IMPORT,
    configE_BLOCK_ENERGY_KVAH_IMPORT,
    configE_BLOCK_ENERGY_KWH_EXPORT,
    configE_BLOCK_ENERGY_KVAH_EXPORT,
    configE_REGISTER_MAX
};

struct MeterRegisterConfigType
{
    MeterRegisterConfigurations Name;
    std::initializer_list<uint8_t> OID;
    float Value;
    int16_t Scalar;
    uint8_t Unit;
};

struct AssociationConfigType
{
    uint16_t ClassID;
    std::initializer_list<uint8_t> OID;
    //std::initializer_list<uint8_t> Attributes;
    //std::initializer_list<uint8_t> Methods;
};

const uint8_t KEY_LEN = 16;
const uint8_t AAD_LEN = 16;

const std::initializer_list<uint8_t> METER_SYSTEM_TITLE = { 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48 };

const std::string ASSOCIATION_MR_AUTHENTICATION_VALUE = "Himanshu";
const std::string ASSOCIATION_US_AUTHENTICATION_VALUE = "wwwwwwwwwwwwwwww";
const std::string ASSOCIATION_FOTA_AUTHENTICATION_VALUE = "Himanshu";
const std::string ASSOCIATION_IHD_AUTHENTICATION_VALUE = "Himanshu";

const uint8_t glo_KEY[KEY_LEN] = {
    //0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    //0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
    'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'
};

const uint8_t glo_AAD[AAD_LEN] = {
    //0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
    //0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf
    'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
    'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'
};

extern std::initializer_list<MeterDataConfigType> MeterDataConfig;
extern std::initializer_list<MeterRegisterConfigType> MeterRegisterConfig;
extern std::initializer_list<AssociationConfigType> AssociationConfigPC;
extern std::initializer_list<AssociationConfigType> AssociationConfigMR;
extern std::initializer_list<AssociationConfigType> AssociationConfigUS;
extern std::initializer_list<AssociationConfigType> AssociationConfigPUSH;
extern std::initializer_list<AssociationConfigType> AssociationConfigFOTA;
extern std::initializer_list<AssociationConfigType> AssociationConfigIHD;
