/*
 * Created by - Himanshu
 * October 21, 2022
 */

#pragma once

#include <cstdint>
#include <ctime>

#include "DLMSValue.h"
#include "interfaces/IClock.h"

namespace ClockStatusBitMask {
	enum ClockStatusBitMask : uint8_t
	{
		invalid_value			= 1 << 0,
		doubtful_value			= 1 << 1,
		different_clock_base	= 1 << 2,
		invalid_clock_status	= 1 << 3,
		daylight_saving_active	= 1 << 7,
		not_specified			= 0xFF
	};
}

struct Date
{
	uint8_t m_YearHighByte	= 0xFF;
	uint8_t m_YearLowByte	= 0xFF;
	uint8_t m_Month			= 0xFF;
	uint8_t m_DayOfMonth	= 0xFF;
	uint8_t m_DayOfWeek		= 0xFF;
} __attribute__ ((__packed__));

struct Time
{
	uint8_t m_Hour				 = 0xFF;
	uint8_t m_Minutes			 = 0xFF;
	uint8_t m_Seconds			 = 0xFF;
	uint8_t m_HundredthsOfSecond = 0xFF;
} __attribute__ ((__packed__));

struct DateTime
{
	Date m_Date;
	Time m_Time;
	uint8_t m_DeviationHighByte	= 0x80;
	uint8_t m_DeviationLowByte	= 0x00;
	uint8_t m_ClockStatus		= ClockStatusBitMask::not_specified;
} __attribute__ ((__packed__));

class COSEMDateTime
{
	bool m_Initialized = false;
	DateTime m_DateTime;

public:
	COSEMDateTime(bool now = false) {
		if (now) {
			m_Initialized = true;
			time_t t = time(0);
			tm* now = localtime(&t);
			m_DateTime.m_Date.m_YearHighByte = now->tm_year >> 8;
			m_DateTime.m_Date.m_YearLowByte = now->tm_year & 0xff;
			m_DateTime.m_Date.m_Month = now->tm_mon + 1;
			m_DateTime.m_Date.m_DayOfMonth = now->tm_mday;
			m_DateTime.m_Date.m_DayOfWeek = now->tm_wday;
			m_DateTime.m_Time.m_Hour = now->tm_hour;
			m_DateTime.m_Time.m_Minutes = now->tm_min;
			m_DateTime.m_Time.m_Seconds = now->tm_sec;
#ifndef _WIN32
			m_DateTime.m_DeviationHighByte = now->tm_gmtoff / 60 >> 8;
			m_DateTime.m_DeviationLowByte = now->tm_gmtoff / 60 & 0xff;
#endif
		}
	}

	~COSEMDateTime() {
	}

	void operator=(EPRI::DLMSValue value) {
		if(value.is<EPRI::DLMSVector>()) {
			m_Initialized = true;
			memcpy(&m_DateTime, value.get<EPRI::DLMSVector>().GetData(), value.get<EPRI::DLMSVector>().Size());
		}
	}

	void operator=(DateTime dateTime) {
		m_DateTime = dateTime;
		m_Initialized = true;
	}

	DateTime GetDateTime() {
		return m_DateTime;
	}

	int16_t GetTimeZone() {
		return (m_DateTime.m_DeviationHighByte << 8) | m_DateTime.m_DeviationLowByte;
	}

	EPRI::DLMSVector GetDaylightSavingsBegin() {
		DateTime dt;
		return EPRI::DLMSVector((uint8_t *)&dt, sizeof(DateTime));
	}

	EPRI::DLMSVector GetDaylightSavingsEnd() {
		DateTime dt;
		return EPRI::DLMSVector((uint8_t *)&dt, sizeof(DateTime));
	}

	uint8_t GetDaylightSavingsDeviation() {
		return 0;
	}

	uint8_t GetClockStatus() {
		return m_DateTime.m_ClockStatus;
	}

	static uint8_t GetClockBase() {
#ifdef STM32
		return EPRI::IClock::ClockBase::INTERNAL_CRYSTAL;
#else
		return EPRI::IClock::ClockBase::NOT_DEFINED;
#endif
	}

	operator EPRI::DLMSVector() const {
		if (not m_Initialized)
			return EPRI::DLMSVector();
		return EPRI::DLMSVector((uint8_t *)&m_DateTime, sizeof(DateTime));
	}
	operator EPRI::DLMSValue() const {
		if (not m_Initialized)
			return EPRI::DLMSBlank;
		return EPRI::DLMSVector(*this);
	}
};