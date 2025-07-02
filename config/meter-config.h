// Himanshu

#pragma once

#include <string>
#include <initializer_list>
#include <vector>
#include <cstdint>

#include "DLMSValue.h"

enum MeterDataConfigurations : uint8_t
{
    configE_CUM_BILLING_COUNT,
    configE_AVAILABLE_BILLING_PERIODS,
    configE_COSEM_LOGICAL_DEVICE_NAME,
    configE_INVOCATION_COUNTER_2,
    configE_INVOCATION_COUNTER_3,
    configE_INVOCATION_COUNTER_4,
    configE_INVOCATION_COUNTER_5,
    configE_CUM_TAMPER_COUNT,
    configE_METER_TYPE,
    configE_METER_CATEGORY,
    configE_CURRENT_RATING,
    configE_EVENT_STATUS_WORD_1,
    configE_ESWF,
    configE_METERING_MODE,
    configE_PAYMENT_MODE,
    configE_LAST_TOKEN_RECHARGE_AMOUNT,
    configE_LAST_TOKEN_RECHARGE_TIME,
    configE_TOTAL_AMOUNT_AT_LAST_RECHARGE,
    configE_CURRENT_BALANCE_AMOUNT,
    configE_CURRENT_BALANCE_TIME,
    configE_METER_SERIAL_NUMBER,
    configE_MANUFACTURER_NAME,
    configE_DEVICE_ID,
    configE_YEAR_OF_MANUFACTURE,
    configE_CUM_PROGRAMMING_COUNT,
    configE_EVENT_VOLTAGE_RELATED,
    configE_EVENT_CURRENT_RELATED,
    configE_EVENT_POWER_RELATED,
    configE_EVENT_TRANSACTION_RELATED,
    configE_EVENT_OTHERS,
    configE_EVENT_NON_ROLL_OVER,
    configE_EVENT_CONTROL,
    configE_MANUFACTURER_SPECIFIC_1,
    configE_METER_FW_VERSION,
    configE_DEMAND_INTEGRATION_PERIOD,
    configE_PROFILE_CAPTURE_PERIOD,
    configE_MANUFACTURER_SPECIFIC_2,
    configE_LOAD_PROFILE_CH_0_RECORDING_INTERVAL_2,
    configE_MANUFACTURER_SPECIFIC_3,
    configE_MANUFACTURER_SPECIFIC_4,
    configE_ACTIVE_RELAY_TIME,
    configE_PASSIVE_RELAY_TIME,
    configE_MANUFACTURER_SPECIFIC_5,

    configE_DATA_MAX
};

struct MeterDataConfigType
{
    MeterDataConfigurations Name;
    std::initializer_list<uint8_t> OID;
    EPRI::DLMSValue Value;
};

// { 0x0000000102FF, "Register Billing Date" },
// { 0x00005E5B0EFF, "Register Cum. Power On Duration" },
// { 0x0100020800FF, "Register Cum. Energy-Wh(Exp)" },
// { 0x01000A0800FF, "Register Cum. Energy-VAh(Exp)" },
// { 0x0100010700FF, "Register Active Power-W" },
// { 0x0100010800FF, "Register Cum. Energy-Wh(Imp)" },
// { 0x0100090700FF, "Register Apparent Power-VA" },
// { 0x0100090800FF, "Register Cum. Energy-VAh(Imp)" },
// { 0x01000B0700FF, "Register Phase Current" },
// { 0x01000C0700FF, "Register Voltage" },
// { 0x01000D0700FF, "Register Signed Power Factor" },
// { 0x01000E0700FF, "Register Frequency-Hz" },
// { 0x01005B0700FF, "Register Neutral Current" },

enum MeterRegisterConfigurations : uint8_t
{
    configE_CONST_ACTIVE_ENERGY,
    configE_AVG_VOLTAGE,
    configE_BLOCK_ENERGY_KWH_IMPORT,
    configE_BLOCK_ENERGY_KVAH_IMPORT,
    configE_BLOCK_ENERGY_KWH_EXPORT,
    configE_BLOCK_ENERGY_KVAH_EXPORT,
    configE_AVG_CURRENT,
    configE_METER_HEALTH_INDICATOR,
    configE_AVG_SIGNAL_STRENGTH,

    configE_BILLING_DATE,
    configE_CUM_POWER_ON_DURATION,
    configE_CUM_ENERGY_WH_EXPORT,
    configE_CUM_ENERGY_VAH_EXPORT,
    configE_ACTIVE_POWER_W,
    configE_CUM_ENERGY_WH_IMPORT,
    configE_APPARENT_POWER_VA,
    configE_CUM_ENERGY_VAH_IMPORT,
    configE_PHASE_CURRENT,
    configE_VOLTAGE,
    configE_SIGNED_POWER_FACTOR,
    configE_FREQUENCY_HZ,
    configE_NEUTRAL_CURRENT,

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
