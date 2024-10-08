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

#include <cstdint>
#include <map>
#include <algorithm>

#include "COSEM/COSEMTypes.h"
#include "Callback.h"
#include "DLMSValue.h"
#include "APDU/APDUFactory.h"
#include "COSEM/COSEMAddress.h"

namespace EPRI
{
    class Transport : public Callback<bool, uint16_t>
    {
        using APDUCallback = Callback<bool, ASN::TagIDType, IAPDUPtr>;

    public:
        enum TransportEvent : uint8_t
        {
            TRANSPORT_CONNECTED    = 1,
            TRANSPORT_DISCONNECTED
        };
        typedef uint16_t ProcessResultType;

        struct DataRequestParameter : public BaseCallbackParameter
        {
            static const uint16_t ID = 0x7000;
            DataRequestParameter()
            {
            }
            DataRequestParameter(COSEMAddressType Src, COSEMAddressType Dest, const DLMSVector& D) :
                SourceAddress(Src), DestinationAddress(Dest), Data(D)
            {
            }
            COSEMAddressType    SourceAddress;
            COSEMAddressType    DestinationAddress;
            DLMSVector          Data;
        };

        typedef Callback<bool, uint16_t, TransportEvent> TransportCallback;

        virtual ~Transport()
        {
        }

        virtual bool IsConnected() const
        {
            return m_Connected;
        }

        virtual ProcessResultType Process() = 0;

        void RegisterTransportEventHandler(COSEMAddressType Address, TransportCallback::CallbackFunction Callback)
        {
            m_Callbacks[Address].m_TransportCallback.RegisterCallback(ID,
                                                                      Callback);
        }

        void RegisterAPDUHandler(COSEMAddressType Address, ASN::TagIDType Tag, APDUCallback::CallbackFunction Callback)
        {
            m_Callbacks[Address].m_APDUCallback.RegisterCallback(Tag,
                                                                 Callback);
        }
        //
        // DATA Service
        //
        virtual bool DataRequest(const DataRequestParameter& Parameters) = 0;

        bool FireTransportEvent(TransportEvent Event)
        {
            m_Connected = (TRANSPORT_CONNECTED == Event);

            std::for_each(m_Callbacks.begin(),
                m_Callbacks.end(),
                [&Event](TransportCallbackType::value_type& Value)
                {
                    bool RetVal;
                    Value.second.m_TransportCallback.FireCallback(ID, Event, &RetVal);
                }
            );
            return true;
        }

    protected:
        virtual bool ProcessReception(COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress, DLMSVector * pAPDUData)
        {
        	LOG_ALL("1:1: ProcessReception\r\n");		// ToDo - check breakpoint
            IAPDUPtr    pAPDU = APDUFactory().Parse(SourceAddress, DestinationAddress, pAPDUData);
            if (pAPDU)
            {
                std::for_each(m_Callbacks.begin(),
                    m_Callbacks.end(),
                    [&pAPDU](TransportCallbackType::value_type& Value)
                    {
                        bool RetVal;
                        LOG_ALL("2: APDU\r\n");		// ToDo - check breakpoint
                        LOG_INFO("TAG TYPE : 0x%02x\r\n\r\n", pAPDU->GetTag());
                        Value.second.m_APDUCallback.FireCallback((ASN::TagIDType)pAPDU->GetTag(),
                            pAPDU,
                            &RetVal);
                        LOG_INFO("ALL WELL THAT ENDS WELL\r\n");
                    });
                return true;
            }
            return false;
        }

        static const uint16_t ID = 0x1742;
        struct TransportCallbacks
        {
            TransportCallback     m_TransportCallback;
            APDUCallback          m_APDUCallback;
        };
        typedef std::map<COSEMAddressType, TransportCallbacks> TransportCallbackType;
        TransportCallbackType                                  m_Callbacks;
        bool                                                   m_Connected = false;
    };

}
