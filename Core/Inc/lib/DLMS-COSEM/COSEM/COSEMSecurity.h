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

#include "APDU/ASNType.h"
#include "APDU/APDUConstants.h"
#include "COSEM/COSEMObjectInstanceID.h"
#include "COSEM/COSEMSecuritySuite.h"
#include "DLMSValue.h"

namespace EPRI
{
    struct SecurityContextType
    {
        enum SecuritySuiteOption : uint8_t
        {
            NO_SUITE = 0,
            SUITE_0 = 1,
            SUITE_1 = 2,
            SUITE_2 = 3
        };

        enum SecurityPolicyBitmask : uint8_t
        {
            no_policy = 0x00,
            authenticated_request = 0x04,
            encrypted_request = 0x08,
            digitally_signed_request = 0x10,
            authenticated_response = 0x20,
            encrypted_response = 0x40,
            digitally_signed_response = 0x80
        };

        SecurityContextType(SecuritySuiteOption Suite, uint8_t Policy);
        virtual ~SecurityContextType();

        const std::shared_ptr<ISecuritySuite> GetSecuritySuite() const;
        void SetSecuritySuite(const ISecuritySuite& riSuite);

        uint8_t GetPolicy() const;
        void ClearPolicy();
        void SetPolicyBit(SecurityPolicyBitmask policy);

        const COSEMObjectInstanceID& GetSecuritySetupObjectID() const;
        void SetSecuritySetupObjectID(const COSEMObjectInstanceID& ID);

    protected:
        uint8_t             m_Policy;
        SecuritySuiteOption m_SuiteOption;
        std::shared_ptr<ISecuritySuite> m_pSecuritySuite;

    };

    struct COSEMSecurityOptions
    {
        using AuthenticationValueType = DLMSValue;
        using APTitleType = DLMSValue;

        static const ASNObjectIdentifier ContextLNRNoCipher;
        static const ASNObjectIdentifier ContextSNRNoCipher;
        static const ASNObjectIdentifier ContextLNRCipher;
        static const ASNObjectIdentifier ContextSNRCipher;

        static const ASNObjectIdentifier MechanismNameNoSecurity;
        static const ASNObjectIdentifier MechanismNameLowLevelSecurity;
        static const ASNObjectIdentifier MechanismNameHighLevelSecurity;
        static const ASNObjectIdentifier MechanismNameHighLevelSecurityMD5;
        static const ASNObjectIdentifier MechanismNameHighLevelSecuritySHA1;
        static const ASNObjectIdentifier MechanismNameHighLevelSecurityGMAC;
        static const ASNObjectIdentifier MechanismNameHighLevelSecuritySHA256;
        static const ASNObjectIdentifier MechanismNameHighLevelSecurityECDSA;

        enum SecurityLevel : uint8_t
        {
            SECURITY_NONE       = 0,
            SECURITY_LOW_LEVEL  = 1,
            SECURITY_HIGH_LEVEL = 2
        };

        COSEMSecurityOptions(SecurityContextType::SecuritySuiteOption Suite = SecurityContextType::NO_SUITE
            , uint8_t Policy = SecurityContextType::no_policy);
        virtual ~COSEMSecurityOptions();

        SecurityLevel Level() const;
        bool LogicalNameReferencing() const;
        bool Authentication() const;
        bool Encryption() const;
        APDUConstants::AuthenticationValueChoice AuthenticationType() const;

        ASNObjectIdentifier     ApplicationContextName;
        ASNObjectIdentifier     MechanismName;
        AuthenticationValueType AuthenticationValue;
        AuthenticationValueType CtoS;           // Received by server from client
        AuthenticationValueType StoC;           // Sent by server to client
        APTitleType             CallingAPTitle;
        APTitleType             RespondingAPTitle = DLMSVector{ METER_SYSTEM_TITLE };
        SecurityContextType     SecurityContext;
    };

}
