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

#include <cstring>

#include "DLMSValue.h"

namespace EPRI
{
    class COSEMObjectInstanceID;

    constexpr uint8_t COSEMValueGroupElements = 6;

    class COSEMObjectInstanceCriteria
    {
        friend class COSEMObjectInstanceID;

    public:
        static constexpr uint16_t ANY = 0x0100;
        static constexpr uint16_t MAX = 0xFFFF;

        struct ValueGroupCriteria
        {
            ValueGroupCriteria()
            {
                Type = TYPE_PRECISE;
                Value.ValueGroup = ANY;
            }
            ValueGroupCriteria(uint16_t VG)
            {
                Type = TYPE_PRECISE;
                Value.ValueGroup = VG;
            }
            ValueGroupCriteria(uint16_t L, uint16_t H)
            {
                Type = TYPE_RANGE;
                Value.ValueRange.Low = L;
                Value.ValueRange.High = H;
            }
            enum CriteriaType
            {
                TYPE_PRECISE,
                TYPE_RANGE
            }                           Type;
            union
            {
                uint16_t ValueGroup;
                struct Range
                {
                    uint16_t Low;
                    uint16_t High;
                }        ValueRange;
            }                           Value;

        };

        COSEMObjectInstanceCriteria() = delete;
        COSEMObjectInstanceCriteria(const COSEMObjectInstanceID& OID);
        COSEMObjectInstanceCriteria(const std::initializer_list<ValueGroupCriteria>& List);
        virtual ~COSEMObjectInstanceCriteria();

        virtual bool Match(const COSEMObjectInstanceID& InstanceID) const;
        
        virtual bool IsPrecise() const
        {
            for (int i = 0; i < COSEMValueGroupElements; i++)
                if (m_Criteria[i].Type != ValueGroupCriteria::TYPE_PRECISE)
                    return false;
            return true;
        }

    protected:
        ValueGroupCriteria m_Criteria[COSEMValueGroupElements];

    };

    class COSEMObjectInstanceID
    {
        using InstanceIDList = std::initializer_list<uint8_t>;

    public:
        COSEMObjectInstanceID();
        COSEMObjectInstanceID(const DLMSVector& Vector);
        COSEMObjectInstanceID(InstanceIDList List);
        COSEMObjectInstanceID(const COSEMObjectInstanceCriteria& Criteria);
        virtual ~COSEMObjectInstanceID();

        enum ValueGroup : uint8_t
        {
            VALUE_GROUP_A = 0,
            VALUE_GROUP_B = 1,
            VALUE_GROUP_C = 2,
            VALUE_GROUP_D = 3,
            VALUE_GROUP_E = 4,
            VALUE_GROUP_F = 5
        };
        inline uint8_t GetValueGroup(ValueGroup Grp) const
        {
            return m_OBIS[Grp];
        }

        bool IsEmpty() const;
        bool Parse(DLMSVector * pVector);
        bool Parse(const std::string& String);
        std::string ToString() const;
        //
        // Operators
        //
        bool operator==(const COSEMObjectInstanceID& RHS) const;
        bool operator!=(const COSEMObjectInstanceID& RHS) const;
        bool operator<(const COSEMObjectInstanceID& RHS) const;
        operator DLMSVector() const;
        operator DLMSValue() const;

    protected:
        uint8_t m_OBIS[COSEMValueGroupElements] = { };

    };

}
