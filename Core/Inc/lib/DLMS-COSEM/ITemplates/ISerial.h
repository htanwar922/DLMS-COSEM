// ===========================================================================
// Copyright (c) 2018, Electric Power Research Institute (EPRI)
// All rights reserved.
//
// DLMS-COSEM ("this software") is licensed under BSD 3-Clause license.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// *  Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// *  Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// *  Neither the name of EPRI nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
// OF SUCH DAMAGE.
//
// This EPRI software incorporates work covered by the following copyright and permission
// notices. You may not use these works except in compliance with their respective
// licenses, which are provided below.
//
// These works are provided by the copyright holders and contributors "as is" and any express or
// implied warranties, including, but not limited to, the implied warranties of merchantability
// and fitness for a particular purpose are disclaimed.
//
// This software relies on the following libraries and licenses:
//
// ###########################################################################
// Boost Software License, Version 1.0
// ###########################################################################
//
// * asio v1.10.8 (https://sourceforge.net/projects/asio/files/)
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// 

#pragma once 

#include <stddef.h>
#include "ERROR_TYPE.h"
#include "ITemplates/ISocket.h"

namespace EPRI
{
    class ISerialSocket;
    
    class ISerial
    {
    public:
        virtual ~ISerial()
        {
        }
        
		typedef struct _Options
		{
			enum BaudRate : uint8_t
			{
				BAUD_300 = 0,
				BAUD_600,
				BAUD_1200,
				BAUD_1800,
				BAUD_2400,
				BAUD_4800,
				BAUD_9600,
				BAUD_19200,
				BAUD_38400,
				BAUD_57600,
				BAUD_115200,
				BAUD_230400,
				BAUD_460800,
				BAUD_500000,
				BAUD_576000,
				BAUD_921600,
				BAUD_1000000,
				BAUD_1152000,
				BAUD_1500000,
				BAUD_2000000,
				BAUD_2500000,
				BAUD_3000000,
				BAUD_3500000,
				BAUD_4000000
			} m_BaudRate;

			uint8_t m_CharacterSize;

			enum Parity : uint8_t
			{
				PARITY_NONE = 0,
				PARITY_EVEN,
				PARITY_ODD
			} m_Parity;

			enum StopBits: uint8_t
			{
				STOPBITS_ONE = 0,
				STOPBITS_ONE_POINT_FIVE,
				STOPBITS_TWO
			} m_StopBits;
			
			const char * m_SSID;
			const char * m_Password;

			_Options(BaudRate Baud = BAUD_115200,
					uint8_t CharacterSize = 8,
					Parity Par = PARITY_NONE,
					StopBits StopBits = STOPBITS_ONE,
					const char * SSID = nullptr,
					const char * Password = nullptr
				)
				: m_BaudRate(Baud)
				, m_CharacterSize(CharacterSize)
				, m_Parity(Par)
				, m_StopBits(StopBits)
				, m_SSID(SSID)
				, m_Password(Password)
			{
			}

		} Options;

        virtual ISerialSocket * CreateSocket(const Options& Opt) = 0;
        virtual void ReleaseSocket(ISerialSocket * pSocket) = 0;
        virtual bool Process() = 0;
        
    };
    
    class ISerialSocket : public ISocket
    {
    public:
        enum FlushDirection
        {
            RECEIVE  = 0,
            TRANSMIT,
            BOTH
        };
        
        virtual ~ISerialSocket()
        {
        }
        
        virtual ERROR_TYPE Flush(FlushDirection Direction) = 0;
        virtual ERROR_TYPE SetOptions(const ISerial::Options& Opt) = 0;
        
    };
	
}
