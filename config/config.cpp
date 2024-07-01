#include "config.h"

const char * addresses[n_METERS] = {
    (char *)"localhost",
    (char *)"10.194.103.225"
};

// "Forward Active Energy" - 3900 - "1-0:0.7.0*255" - "VAh"
// "Forward Apparent Energy" - 3898 - "1-0:0.7.2*255" - "Wh"
// 0 - Energy from the dataset file 300_1_original_data.csv is kWh, and not Wh

const uint8_t AvailableRegisterOBISGroupC[27] = {
    9, 1, 13, 12, 1,
    29, 21, 33, 124, 32, 31,
    49, 41, 53, 125, 52, 51,
    69, 61, 73, 126, 72, 71,
    14, 0
};

const char * RegisterParamName[27] = {
    "Apparent Power", "Active Power", "PF", "Phase Voltage", "Avg Current",
    "Apparent Power (R)", "Active Power (R)", "PF (R)", "Voltage (R-Y)", "Voltage (R)", "Current (R)",
    "Apparent Power (Y)", "Active Power (Y)", "PF (Y)", "Voltage (Y-B)", "Voltage (R)", "Current (Y)",
    "Apparent Power (B)", "Active power (B)", "PF (B)", "Voltage (B-R)", "Voltage (R)", "Current (B)",
    "Frequency", "Forward Active Energy",
};

std::vector<uint8_t> SelectedRegistersOBISGroupC = {
    33, 0, 29, 31, 21, 32, 14
};

const uint16_t MeterRegisterAddress[27] = {
    3840, 3842, 3846, 3848, 3850, 3852,     // Average
    3856, 3858, 3862, 3864, 3866, 3868,     // R-phase, R-Y
    3870, 3872, 3876, 3878, 3880, 3882,     // Y-phase, Y-B
    3884, 3886, 3890, 3892, 3894, 3896,     // B-phase, B-R
    3854, 3900,
};

const char * MeterRegisterUnits[27] = {
    "VA", "W", "", "V", "A", "A",
    "VA", "W", "", "V", "V", "A",
    "VA", "W", "", "V", "V", "A",
    "VA", "W", "", "V", "V", "A",
    "Hz", "Wh",
};

int FindMeterRegisterByOBISGroupC(uint8_t OBISGroupC)
{
    for (int i = 0; i < sizeof(AvailableRegisterOBISGroupC); i++)
    {
        if (AvailableRegisterOBISGroupC[i] == OBISGroupC)
        {
            return MeterRegisterAddress[i];
        }
    }
    return -1;
}

const char * FindRegisterParamNameByOBISGroupC(uint8_t OBISGroupC)
{
    for (int i = 0; i < sizeof(AvailableRegisterOBISGroupC); i++)
    {
        if (AvailableRegisterOBISGroupC[i] == OBISGroupC)
        {
            return RegisterParamName[i];
        }
    }
    return "";
}

const std::string packet_types[2] = {"Raw", "Compressed"};

#if !configM_REAL_MODBUS_DEVICE

float GetRegisterDefaultValue(int reg)
{
    switch (reg)
    {
    case 3900:
        return 0.0;
    case 3854:
        return 50.0;
    case 3866:
    case 3880:
    case 3894:
        return 230.0;
    case 3858:
    case 3872:
    case 3886:
        return 1.0;
    default:
        return -1.0;
    }
}

#endif

/*
- ACCESS Service inputs for 33, 0, 29, 31, 21, 32, 14
7
1
3
2
1-0:33.7.0*255
1
3
2
1-0:0.7.0*255
1
3
2
1-0:29.7.0*255
1
3
2
1-0:31.7.0*255
1
3
2
1-0:21.7.0*255
1
3
2
1-0:32.7.0*255
1
3
2
1-0:14.7.2*255
*/