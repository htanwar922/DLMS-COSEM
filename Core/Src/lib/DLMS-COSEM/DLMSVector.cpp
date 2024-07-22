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

#include <cstring>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <type_traits>
#include <stdexcept>

#include "DLMSVector.h"

namespace EPRI
{
    DLMSVector::DLMSVector()
    {
    }

    DLMSVector::DLMSVector(size_t Size)
    {
        m_Data.resize(Size);
    }
    
    DLMSVector::DLMSVector(const std::initializer_list<uint8_t>& Value)
    {
        m_Data = Value;
    }

    DLMSVector::DLMSVector(const DLMSVector& Value, size_t Start /*= 0*/, size_t End /*= 0*/)
    {
        if (Start == 0 and End == 0)
        {
            m_Data = Value.m_Data;
        }
        else {
            if (End == 0)
            {
                End = Value.m_Data.size();
            }
            if (0 <= Start and Start < End and End <= Value.m_Data.size())
            {
                m_Data.assign(Value.m_Data.begin() + Start, Value.m_Data.begin() + End);
            }
            else
            {
                throw std::out_of_range("Invalid start or end index");
            }
        }
    }
    
    DLMSVector::DLMSVector(const std::vector<uint8_t>& Value)
    {
        m_Data = Value;    
    }

    DLMSVector::DLMSVector(const void * pBuffer, size_t Size)
    {
        const uint8_t * p = static_cast<const uint8_t *>(pBuffer);
        m_Data.insert(m_Data.end(), p, p + Size);
    }
    
    DLMSVector::~DLMSVector()
    {
    }

    size_t DLMSVector::Size() const
    {
        return m_Data.size();
    }

    size_t DLMSVector::GetReadPosition() const
    {
        return m_ReadPosition;
    }

    bool DLMSVector::SetReadPosition(size_t Value)
    {
        if (Value >= m_Data.size())
            return false;
        m_ReadPosition = Value;
        return true;
    }
    
    bool DLMSVector::IsAtEnd() const
    {
        return m_ReadPosition >= m_Data.size();
    }

    bool DLMSVector::Skip(size_t Count)
    {
        return SetReadPosition(m_ReadPosition + Count);
    }
    
    bool DLMSVector::Zero(size_t Position /* = 0 */, size_t Count /* = 0 */)
    {
        if (0 == Count)
        {
            Count = m_Data.size() - Position;
        }
        if (Position + Count > m_Data.size())
        {
            return false;
        }
        std::memset(m_Data.data() + Position, '\0', Count);
        return true;
    }

    void DLMSVector::RemoveReadBytes()
    {
        if (m_ReadPosition < m_Data.size())
        {
            std::vector<uint8_t>(m_Data.begin() + m_ReadPosition, m_Data.end()).swap(m_Data);
            m_ReadPosition = 0;
        }
    }

    void DLMSVector::Resize(size_t Size)
    {
        m_Data.resize(m_ReadPosition + Size);
    }

    size_t DLMSVector::AppendFloat(float Value)
    {
        size_t RetVal = m_Data.size();
        union
        {
            float   Float;
            uint8_t Bytes[sizeof(float)];
        } Convert;
        Convert.Float = Value;
        m_Data.push_back(Convert.Bytes[3]);
        m_Data.push_back(Convert.Bytes[2]);
        m_Data.push_back(Convert.Bytes[1]);
        m_Data.push_back(Convert.Bytes[0]);
        return RetVal;
    }

    size_t DLMSVector::AppendDouble(double Value)
    {
        size_t RetVal = m_Data.size();
        union
        {
            double  Double;
            uint8_t Bytes[sizeof(double)];
        } Convert;
        Convert.Double = Value;
        m_Data.push_back(Convert.Bytes[7]);
        m_Data.push_back(Convert.Bytes[6]);
        m_Data.push_back(Convert.Bytes[5]);
        m_Data.push_back(Convert.Bytes[4]);
        m_Data.push_back(Convert.Bytes[3]);
        m_Data.push_back(Convert.Bytes[2]);
        m_Data.push_back(Convert.Bytes[1]);
        m_Data.push_back(Convert.Bytes[0]);
        return RetVal;
    }

    size_t DLMSVector::AppendBuffer(const void * pValue, size_t Count)
    {
        size_t RetVal = m_Data.size();
        const uint8_t * p = static_cast<const uint8_t *>(pValue);
        m_Data.insert(m_Data.end(), p, p + Count);
        return RetVal;
    }

    ssize_t DLMSVector::Append(const DLMSVector& Value, size_t Position /* = 0 */, size_t Count /* = 0 */)
    {
        ssize_t RetVal = m_Data.size();
        if (0 == Count)
        {
            Count = Value.Size() - Position;
        }
        if (Position + Count > Value.Size())
            return -1;
        AppendBuffer(Value.m_Data.data() + Position, Count);
        return RetVal;
    }
    
    ssize_t DLMSVector::Append(DLMSVector * pValue, size_t Count /* = 0 */)
    {
        ssize_t RetVal = m_Data.size();
        if (0 == Count)
        {
            Count = pValue->Size() - pValue->GetReadPosition();
        }
        if (pValue->GetReadPosition() + Count > pValue->Size())
            return -1;
        size_t Position = AppendExtra(Count);
        if (!pValue->GetBuffer(&m_Data[Position], Count))
        {
            RetVal = -1;
        }
        return RetVal;
    }

    size_t DLMSVector::Append(const std::string& Value)
    {
        size_t RetVal = m_Data.size();
        m_Data.insert(m_Data.end(), Value.begin(), Value.end());
        return RetVal;
    }
    
    size_t DLMSVector::Append(const std::vector<uint8_t>& Value)
    {
        size_t RetVal = m_Data.size();
        m_Data.insert(m_Data.end(), Value.begin(), Value.end());
        return RetVal;
    }
    
    size_t DLMSVector::AppendExtra(size_t Count)
    {
        size_t RetVal = m_Data.size();
        m_Data.resize(RetVal + Count);
        return RetVal;
    }

    void DLMSVector::Clear()
    {
        m_Data.clear();
        m_ReadPosition = 0;
    }

    bool DLMSVector::Get(std::string * pValue, size_t Count, bool Append /*= false*/)
    {
        if (m_ReadPosition + Count <= m_Data.size())
        {
            if (!Append)
            {
                pValue->clear();
            }
            std::string::iterator it = Append ? pValue->end() : pValue->begin();
            pValue->insert(it,
                m_Data.begin() + m_ReadPosition,
                m_Data.begin() + m_ReadPosition + Count);
            m_ReadPosition += Count;
            return true;
        }
        return false;
    }

    bool DLMSVector::GetBuffer(uint8_t * pValue, size_t Count)
    {
        if (m_ReadPosition + Count <= m_Data.size())
        {
            std::memcpy(pValue, &m_Data.data()[m_ReadPosition], Count);
            m_ReadPosition += Count;
            return true;
        }
        return false;
    }

    bool DLMSVector::GetVector(DLMSVector * pValue, size_t Count)
    {
        if (m_ReadPosition + Count <= m_Data.size())
        {
            pValue->m_Data.insert(pValue->m_Data.end(),
                m_Data.begin() + m_ReadPosition, 
                m_Data.begin() + m_ReadPosition + Count);
            m_ReadPosition += Count;
            return true;
        }
        return false;
    }

    std::vector<uint8_t> DLMSVector::GetBytes() const
    {
        return m_Data;
    }
    
    const uint8_t * DLMSVector::GetData() const
    {
        return m_Data.data();
    }
    
    int DLMSVector::PeekByte(size_t OffsetFromGetPosition /* = 0 */) const
    {
        if (m_ReadPosition + OffsetFromGetPosition < m_Data.size())
        {
            return m_Data[m_ReadPosition + OffsetFromGetPosition];
        }
        return -1;
    }
    
    int DLMSVector::PeekByteAtEnd(size_t OffsetFromEndOfVector /* = 0*/) const
    {
        ssize_t Index = (ssize_t) m_Data.size() - (ssize_t) OffsetFromEndOfVector - 1;
        if (Index >= 0)
        {
            return m_Data[Index];
        }
        return -1;
    }  
    
    bool DLMSVector::PeekBuffer(uint8_t * pValue, size_t Count) const
    {
        if (m_ReadPosition + Count <= m_Data.size())
        {
            std::memcpy(pValue, &m_Data[m_ReadPosition], Count);
            return true;
        }
        return false;
    }

    std::string DLMSVector::ToString() const
    {
        std::ostringstream Output;
        std::for_each(m_Data.begin(), m_Data.end(),
            [&Output](uint8_t Value)
            {
                Output << std::setw(2) << Value << ' ';
            });
        return Output.str();
    }

    DLMSVector& DLMSVector::operator=(DLMSVector& lval)
    {
        m_Data = lval.m_Data;
        m_ReadPosition = 0;
        return *this;
    }

    DLMSVector& DLMSVector::operator=(const DLMSVector& lval)
    {
        m_Data = lval.m_Data;
        m_ReadPosition = 0;
        return *this;
    }
   
    uint8_t& DLMSVector::operator[](size_t Index)
    {
        return m_Data[Index];
    }

    const uint8_t& DLMSVector::operator[](size_t Index) const
    {
        return m_Data[Index];
    }
    
    bool DLMSVector::operator==(const DLMSVector& rhs) const
    {
        return rhs.m_Data == m_Data;
    }

    bool DLMSVector::operator!=(const DLMSVector& rhs) const
    {
        return rhs.m_Data != m_Data;
    }

}
