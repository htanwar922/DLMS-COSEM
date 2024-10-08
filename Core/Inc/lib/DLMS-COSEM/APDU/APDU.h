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
#include <vector>
#include <memory>

#include "APDUComponent.h"
#include "COSEM/COSEMAddress.h"
#include "COSEM/COSEMSecurity.h"
#include "COSEM/COSEMSecuritySuite.h"

namespace EPRI
{
    class IAPDU
    {
    public:
        virtual ~IAPDU()
        {
        }
        virtual void RegisterComponent(IAPDUComponent * pComponent) = 0;
        virtual std::vector<uint8_t> GetBytes() = 0;
        virtual bool Parse(DLMSVector * pData,
            COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress) = 0;
        virtual bool IsValid() const = 0;
        virtual ASN::TagIDType GetTag() = 0;
        virtual void Clear() = 0;
        virtual COSEMAddressType GetSourceAddress()
        {
            return m_SourceAddress;
        }

        virtual COSEMAddressType GetDestinationAddress()
        {
            return m_DestinationAddress;
        }

    protected:
        COSEMAddressType m_SourceAddress = ReservedAddresses::NO_STATION;
        COSEMAddressType m_DestinationAddress = ReservedAddresses::NO_STATION;

    };

    typedef std::unique_ptr<IAPDU> IAPDUPtr;

    template <ASN::TagIDType TAG>
    class APDU : public IAPDU
    {
        using ComponentVector = std::vector<IAPDUComponent *>;

    public:
        static const ASN::TagIDType Tag = TAG;

        virtual ~APDU()
        {
        }
        //
        // IAPDU
        //
        virtual ASN::TagIDType GetTag()
        {
            return Tag;
        }

        virtual void Clear()
        {
            for (int Index = 0; Index < m_Components.size(); ++Index)
            {
                m_Components[Index]->Clear();
            }
        }

        virtual void RegisterComponent(IAPDUComponent * pComponent)
        {
            m_Components.push_back(pComponent);
        }

        virtual std::vector<uint8_t> GetBytes()
        {
            DLMSVector RetVal;
            DLMSVector ComponentData;

            RetVal.Append<uint8_t>(Tag);
            for (uint32_t Index = 0; Index < m_Components.size(); ++Index)
            {
                if (!m_Components[Index]->AppendToVector(&ComponentData))
                {
                    return std::vector<uint8_t>();
                }
            }
            ASNType::AppendLength(ComponentData.Size(), &RetVal);
            RetVal.Append(ComponentData);

            return RetVal.GetBytes();
        }

        virtual bool IsValid() const
        {
            bool RetVal = false;
            for (int Index = 0; Index < m_Components.size(); ++Index)
            {
                if (!(RetVal = m_Components[Index]->IsValid()))
                {
                    break;
                }
            }
            return RetVal;
        }

        virtual bool Parse(DLMSVector * pData,
            COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress)
        {
            bool   RetVal = false;
            size_t Length = 0;

            if (Tag == pData->Peek<uint8_t>() &&
                pData->Skip(sizeof(uint8_t)) &&
                ASNType::GetLength(pData, &Length) &&
                (pData->Size() - pData->GetReadPosition()) >= Length)
            {
                size_t                    ComponentsProcessed = 0;
                ComponentVector::iterator it = m_Components.begin();
                while (it != m_Components.end())
                {
                    RetVal = (*it++)->Parse(pData);
                    if (RetVal)
                    {
                        ComponentsProcessed++;
                    }
                }
                RetVal = ComponentsProcessed;
            }
            if (RetVal)
            {
                m_SourceAddress = SourceAddress;
                m_DestinationAddress = DestinationAddress;
            }
            return RetVal;
        }

    protected:
        APDU()
        {
        }

        ComponentVector  m_Components;

    };

    template <ASN::TagIDType TAG>
    class APDUSingleType : public IAPDU
    {
    public:
        static const ASN::TagIDType Tag = TAG;

        APDUSingleType() = delete;
        virtual ~APDUSingleType()
        {
        }
        //
        // IAPDU
        //
        virtual ASN::TagIDType GetTag()
        {
            return Tag;
        }

        virtual void Clear()
        {
            m_Type.Clear();
        }

        virtual std::vector<uint8_t> GetBytes()
        {
            DLMSVector RetVal;
            RetVal.Append<uint8_t>(Tag);
            RetVal.Append(m_Type.GetBytes());
            return RetVal.GetBytes();
        }

        virtual bool Parse(DLMSVector * pData,
            COSEMAddressType SourceAddress = ReservedAddresses::NO_STATION,
            COSEMAddressType DestinationAddress = ReservedAddresses::NO_STATION)
        {
            if (Tag == pData->Peek<uint8_t>() &&
                pData->Skip(sizeof(uint8_t)))
            {
                if (m_Type.Parse(pData))
                {
                    m_SourceAddress = SourceAddress;
                    m_DestinationAddress = DestinationAddress;
                    return true;
                }
            }
            return false;
        }

        // NOTE: DERIVED CLASSES MUST IMPLEMENT IsValid()

    protected:
        APDUSingleType(ASN::SchemaEntryPtr SchemaEntry) :
            m_Type(SchemaEntry)
        {
        }

        ASNType          m_Type;

    private:
        virtual void RegisterComponent(IAPDUComponent * /*pComponent*/) final
        {
            // NOT IMPLEMENTED AND VISIBILITY CHANGE
        }

    };

    namespace GLO {
        extern const ASN::SchemaEntry Ciphered_APDU_Schema[];

        template <ASN::TagIDType TAG>
        class CipheredBase : public APDUSingleType<TAG>
        {
        public:
            virtual ~CipheredBase()
            {
            }

            virtual std::vector<uint8_t> GetBytes() final
            {
                if (Serialize())
                {
                    return CipheredBase::APDUSingleType::GetBytes();
                }
                return std::vector<uint8_t>();
            }

            bool Parse(DLMSVector* pData,
                COSEMAddressType SourceAddress = ReservedAddresses::NO_STATION,
                COSEMAddressType DestinationAddress = ReservedAddresses::NO_STATION)
            {
                if (APDUSingleType<TAG>::Parse(pData, SourceAddress, DestinationAddress))
                {
                    if (Deserialize())
                    {
                        return (m_Initialized = true);
                    }
                }
                return false;
            }

            bool Initialized() const
            {
                return m_Initialized;
            }

            uint8_t GetSecurityControlByte() const
            {
                return m_SecurityControlByte;
            }

            uint32_t GetInvocationCounter() const
            {
                return m_InvocationCounter;
            }

            const DLMSVector& GetCipheredDataAndAuthenticationTag() const
            {
                return m_CipheredDataAndAuthenticationTag;
            }

            DLMSVector Encrypt(const std::shared_ptr<ISecuritySuite> pSuite, const COSEMSecurityOptions& SecurityOptions, const DLMSVector& Plaintext)
            {
#if MODE == SERVER
                DLMSVector IV(METER_SYSTEM_TITLE);
#else
                DLMSVector IV(SecurityOptions.CallingAPTitle);
#endif
                pSuite->IncrementInvocationCounter();
                IV.Append(m_InvocationCounter = pSuite->GetInvocationCounter());
                DLMSVector Ciphertext;
                DLMSVector Tag;
                if (pSuite->Encrypt(Plaintext, IV, Ciphertext, Tag))
                {
                    Ciphertext.Append(Tag);
                    m_SecurityControlByte = pSuite->GetSecurityControlByte();
                    m_CipheredDataAndAuthenticationTag = Ciphertext;
                    m_Initialized = true;
                    return m_CipheredDataAndAuthenticationTag;
                }
                return DLMSVector();
            }

            DLMSVector Decrypt(const std::shared_ptr<ISecuritySuite> pSuite, const COSEMSecurityOptions& SecurityOptions) const
            {
#if MODE == SERVER
                DLMSVector IV(SecurityOptions.CallingAPTitle);
#else
                DLMSVector IV(METER_SYSTEM_TITLE);
#endif
                IV.Append(m_InvocationCounter);
                DLMSVector Ciphertext = m_CipheredDataAndAuthenticationTag;
                DLMSVector Tag(Ciphertext, Ciphertext.Size() - pSuite->GetTagLength());
                Ciphertext.Resize(Ciphertext.Size() - pSuite->GetTagLength());
                DLMSVector Plaintext;
                pSuite->ClearSecurityControlByte();
                pSuite->SetSecurityControlBits((ISecuritySuite::SecurityControlBitMask)m_SecurityControlByte);
                if (pSuite->Decrypt(Ciphertext, IV, Plaintext, Tag))
                {
                    return Plaintext;
                }
                return DLMSVector();
            }
        protected:
            CipheredBase()
                : CipheredBase::APDUSingleType(Ciphered_APDU_Schema)
            {
            }
            CipheredBase(const CipheredBase& Request)
                : CipheredBase::APDUSingleType(Ciphered_APDU_Schema)
            {
                m_SecurityControlByte = Request.m_SecurityControlByte;
                m_InvocationCounter = Request.m_InvocationCounter;
                m_CipheredDataAndAuthenticationTag = Request.m_CipheredDataAndAuthenticationTag;
                m_Initialized = Request.m_Initialized;
            }
            virtual bool Deserialize()
            {
                DLMSValue Value;
                try
                {
                    this->m_Type.Rewind();
                    if (ASNType::GetNextResult::VALUE_RETRIEVED != this->m_Type.GetNextValue(&Value))
                        return false;
                    size_t Length = Value.get<size_t>();
                    if (ASNType::GetNextResult::VALUE_RETRIEVED != this->m_Type.GetNextValue(&Value))
                        return false;
                    m_SecurityControlByte = DLMSValueGet<uint8_t>(Value);
                    if (ASNType::GetNextResult::VALUE_RETRIEVED != this->m_Type.GetNextValue(&Value))
                        return false;
                    m_InvocationCounter = DLMSValueGet<uint32_t>(Value);
                    if (ASNType::GetNextResult::VALUE_RETRIEVED != this->m_Type.GetNextValue(&Value))
                        return false;
                    m_CipheredDataAndAuthenticationTag = DLMSValueGet<DLMSVector>(Value);
                    if (Length != sizeof m_SecurityControlByte + sizeof m_InvocationCounter + m_CipheredDataAndAuthenticationTag.Size())
                        return false;
                    return true;
                }
                catch (const std::exception&)
                {
                }
                return false;
            }
            virtual bool Serialize()
            {
                if (not m_Initialized)
                    return false;
                this->m_Type.Clear();
                this->m_Type.Append(sizeof m_SecurityControlByte + sizeof m_InvocationCounter + m_CipheredDataAndAuthenticationTag.Size());
                this->m_Type.Append(m_SecurityControlByte);
                this->m_Type.Append(m_InvocationCounter);
                this->m_Type.Append(m_CipheredDataAndAuthenticationTag);
                return true;
            }
            //
            // IAPDU
            //
            virtual bool IsValid() const final
            {
                return true;
            }

            uint8_t m_SecurityControlByte = 0x00;
            uint32_t m_InvocationCounter = 0;
            DLMSVector m_CipheredDataAndAuthenticationTag;
            bool m_Initialized = false;
        };

        class General_Glo_Ciphering : public APDUSingleType<0xdb>
        {
            ASN_DEFINE_SCHEMA(General_Glo_Ciphering_Schema)

            struct Ciphered : public CipheredBase<General_Glo_Ciphering::Tag>
            {
                Ciphered() = default;
                Ciphered(const Ciphered& Request) : Ciphered::CipheredBase(Request)
                {
                }
                virtual ~Ciphered()
                {
                }
            };
        public:
            General_Glo_Ciphering();
            virtual ~General_Glo_Ciphering();

            virtual bool IsValid() const;
            virtual bool Parse(DLMSVector* pData,
                COSEMAddressType SourceAddress = ReservedAddresses::NO_STATION,
                COSEMAddressType DestinationAddress = ReservedAddresses::NO_STATION);
            virtual std::vector<uint8_t> GetBytes() final;

            DLMSVector Encrypt(const std::shared_ptr<ISecuritySuite> pSuite, const COSEMSecurityOptions& SecurityOptions, const DLMSVector& Plaintext);
            DLMSVector Decrypt(const std::shared_ptr<ISecuritySuite> pSuite, const COSEMSecurityOptions& SecurityOptions) const;

        protected:
            DLMSVector  m_SystemTitle;
            Ciphered    m_Ciphered;
            bool        m_Initialized = false;

            virtual bool Deserialize();
            virtual bool Serialize();
        };
    }
}
