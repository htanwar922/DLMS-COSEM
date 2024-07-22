// Himanshu

#pragma once

#include <string>
#include <initializer_list>
#include <vector>
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

struct AssociationConfigType
{
    uint16_t ClassID;
    std::initializer_list<uint8_t> OID;
    //std::initializer_list<uint8_t> Attributes;
    //std::initializer_list<uint8_t> Methods;
};

const std::initializer_list<uint8_t> METER_SYSTEM_TITLE = { 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48 };

const uint8_t KEY_LEN = 16;
const uint8_t AAD_LEN = 16;

const uint8_t glo_KEY[KEY_LEN] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

const uint8_t client_system_title[8] = {
    0x4d, 0x4d, 0x4d, 0x00, 0x00, 0xbc, 0x61, 0x4e			// DLMS-UA
    // 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48			// Gurux
};
const uint8_t server_system_title[8] = {
    // 0x4d, 0x4d, 0x4d, 0x00, 0x00, 0xbc, 0x61, 0x4e			// DLMS-UA
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48			// Gurux
};

const uint8_t glo_AAD[AAD_LEN] = {
    //0x00,	// Placeholder for the Security Control field
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
    0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf
};

extern std::initializer_list<MeterConfigType> MeterConfig;
extern std::initializer_list<AssociationConfigType> AssociationConfigPC;
extern std::initializer_list<AssociationConfigType> AssociationConfigMR;
extern std::initializer_list<AssociationConfigType> AssociationConfigUS;
extern std::initializer_list<AssociationConfigType> AssociationConfigPUSH;
extern std::initializer_list<AssociationConfigType> AssociationConfigFOTA;
extern std::initializer_list<AssociationConfigType> AssociationConfigIHD;

const std::string ASSOCIATION_MR_AUTHENTICATION_VALUE = "Himanshu";
