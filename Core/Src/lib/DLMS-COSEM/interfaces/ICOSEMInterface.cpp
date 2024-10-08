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

#include "interfaces/ICOSEMInterface.h"

namespace EPRI
{
    //
    // ICOSEM
    //
    bool ICOSEM::RegisterObjectInstanceID(const COSEMObjectInstanceID& ObjectInstanceID)
    {
        if (m_ObjectValueMap.find(ObjectInstanceID) == m_ObjectValueMap.end())
        {
            m_ObjectValueMap[ObjectInstanceID] = {};
            return true;
        }
        return false;
    }

    ICOSEM::COSEMObjectInstanceIDList ICOSEM::GetObjectInstanceIDList() const
    {
        COSEMObjectInstanceIDList ObjectInstanceIDList;
        for (const auto& it : m_ObjectValueMap)
        {
            ObjectInstanceIDList.insert(it.first);
        }
        return ObjectInstanceIDList;
    }

    void ICOSEM::SetCaptureValue(const COSEMObjectInstanceID& ObjectId, const DLMSValue& Value)
    {
        CheckExists(ObjectId);
        m_ObjectValueMap[ObjectId].capture_value = Value;
    }

    DLMSValue ICOSEM::GetCaptureValue(const COSEMObjectInstanceID& ObjectId) const
    {
        CheckExists(ObjectId);
        return m_ObjectValueMap.at(ObjectId).capture_value;
    }

    void ICOSEM::SetAttributeAccessRights(COSEMObjectInstanceID ObjectId, ObjectAttributeIdType AttributeId, uint8_t AccessRights)
    {
        CheckExists(ObjectId);
        m_ObjectValueMap[ObjectId].attribute_access[AttributeId] = AccessRights;
    }

    void ICOSEM::SetMethodAccessRights(COSEMObjectInstanceID ObjectId, ObjectAttributeIdType MethodId, uint8_t AccessRights)
    {
        CheckExists(ObjectId);
        m_ObjectValueMap[ObjectId].method_access[MethodId] = AccessRights;
    }

    uint8_t ICOSEM::GetAttributeAccessRights(COSEMObjectInstanceID ObjectId, ObjectAttributeIdType AttributeId) const
    {
        if (m_ObjectValueMap.find(ObjectId) == m_ObjectValueMap.end())
            return 0;
        if (m_ObjectValueMap.at(ObjectId).attribute_access.find(AttributeId) == m_ObjectValueMap.at(ObjectId).attribute_access.end())
            return 0;
        return m_ObjectValueMap.at(ObjectId).attribute_access.at(AttributeId);
    }

    uint8_t ICOSEM::GetMethodAccessRights(COSEMObjectInstanceID ObjectId, ObjectAttributeIdType MethodId) const
    {
        if (m_ObjectValueMap.find(ObjectId) == m_ObjectValueMap.end())
            return 0;
        if (m_ObjectValueMap.at(ObjectId).method_access.find(MethodId) == m_ObjectValueMap.at(ObjectId).method_access.end())
            return 0;
        return m_ObjectValueMap.at(ObjectId).method_access.at(MethodId);
    }

    DLMSStructure ICOSEM::GetAccessRights(COSEMObjectInstanceID ObjectId) const
    {
        DLMSStructure data;
        DLMSArray array;
        for (COSEMAttributeMap::value_type attribute : m_Attributes)
        {
            array.push_back(DLMSStructure({
                (int8_t)attribute.first,
                (uint8_t)GetAttributeAccessRights(ObjectId, attribute.first),
                DLMSBlank
            }));
        }
        data.push_back(array);

        array.clear();
        for (COSEMMethodMap::value_type method : m_Methods)
        {
            array.push_back(DLMSStructure({
                (int8_t)method.first,
                (uint8_t)GetMethodAccessRights(ObjectId, method.first)
            }));
        }
        data.push_back(array);

        return data;
    }

    void ICOSEM::CheckExists(const COSEMObjectInstanceID& ObjectId) const
    {
        if (m_ObjectValueMap.find(ObjectId) == m_ObjectValueMap.end())
        {
            throw std::runtime_error("ObjectID not found");
        }
    }
    //
    // ICOSEMInterface
    //
    ICOSEMInterface::ICOSEMInterface(uint16_t class_id,
            uint8_t version,
            uint16_t CardinalityMin /* = 0 */,
            uint16_t CardinalityMax /* = std::numeric_limits<uint16_t>::max() */) :
        m_class_id(class_id),
        m_version(version),
        m_CardinalityMin(CardinalityMin),
        m_CardinalityMax(CardinalityMax)
    {
        COSEM_BEGIN_ATTRIBUTES
            COSEM_ATTRIBUTE(logical_name)
        COSEM_END_ATTRIBUTES
    }

    ICOSEMInterface::~ICOSEMInterface()
    {
    }

    uint16_t ICOSEMInterface::GetPropertyValue(InterfaceProperties Property) const
    {
        switch (Property)
        {
        case CLASS_ID:
            return m_class_id;
        case VERSION:
            return m_version;
        case CARDINALITY_MIN:
            return m_CardinalityMin;
        case CARDINALITY_MAX :
            return m_CardinalityMax;
        default:
            break;
        }
        return 0;
    }

    // Himanshu
    uint16_t ICOSEMInterface::GetClassID() const
    {
        return m_class_id;
    }

    // Himanshu
    uint8_t ICOSEMInterface::GetVersion() const
    {
        return m_version;
    }

    void ICOSEMInterface::RegisterAttribute(ICOSEMAttribute * pAttribute)
    {
        pAttribute->m_pInterface = this;
        m_Attributes[pAttribute->AttributeID] = pAttribute;
    }

    void ICOSEMInterface::RegisterMethod(ICOSEMMethod * pMethod)
    {
        pMethod->m_pInterface = this;
        m_Methods[pMethod->MethodID] = pMethod;
    }

    ICOSEMAttribute * ICOSEMInterface::FindAttribute(ObjectAttributeIdType AttributeId) const
    {
        COSEMAttributeMap::const_iterator it = m_Attributes.find(AttributeId);
        if (it != m_Attributes.cend())
        {
            return it->second;
        }
        return nullptr;
    }

    ICOSEMMethod * ICOSEMInterface::FindMethod(ObjectAttributeIdType MethodId) const
    {
        COSEMMethodMap::const_iterator it = m_Methods.find(MethodId);
        if (it != m_Methods.cend())
        {
            return it->second;
        }
        return nullptr;
    }
    //
    // ICOSEMObject
    //
    ICOSEMObject::ICOSEMObject(const COSEMObjectInstanceCriteria& Criteria,
        uint16_t ShortNameBase /* = std::numeric_limits<uint16_t>::max()*/) :
        m_InstanceCriteria(Criteria),
        m_ShortNameBase(ShortNameBase)
    {
        if (Criteria.IsPrecise() and not COSEMObjectInstanceID(Criteria).IsEmpty()) {
            RegisterObjectInstanceID(Criteria);
            SetAttributeAccessRights(Criteria, ICOSEMInterface::LOGICAL_NAME, 0x01);    // read-access
            for (const COSEMAttributeMap::value_type& attribute : m_Attributes)
                SetAttributeAccessRights(Criteria, attribute.first, 0);
            for (const COSEMMethodMap::value_type& method : m_Methods)
                SetMethodAccessRights(Criteria, method.first, 0);
        }
        else {
            LOG_ALERT("Call RegisterObjectInstanceID,"
                " SetAttributeAccessRights and SetMethodAccessRights"
                " if using Value-Groups or empty Instance ID"
                " in COSEM Object initialization\r\n"
            );
        }
    }

    ICOSEMObject::~ICOSEMObject()
    {
    }

    bool ICOSEMObject::Supports(const Cosem_Attribute_Descriptor& Descriptor) const
    {
        if (m_InstanceCriteria.Match(Descriptor.instance_id))
        {
            const ICOSEMInterface * pInterface = dynamic_cast<const ICOSEMInterface *>(this);
            if (pInterface &&
                Descriptor.class_id == pInterface->m_class_id &&
                pInterface->HasAttribute(Descriptor.attribute_id))
            {
                for (const std::pair<COSEMObjectInstanceID, COSEMObjectValueType>& ObjectValue : m_ObjectValueMap) {
                    if (COSEMObjectInstanceCriteria(ObjectValue.first).Match(Descriptor.instance_id))
                        return true;
                }
            }
        }
        return false;
    }

    bool ICOSEMObject::Supports(const Cosem_Method_Descriptor& Descriptor) const
    {
        if (m_InstanceCriteria.Match(Descriptor.instance_id))
        {
            const ICOSEMInterface * pInterface = dynamic_cast<const ICOSEMInterface *>(this);
            if (pInterface &&
                Descriptor.class_id == pInterface->m_class_id &&
                pInterface->HasMethod(Descriptor.method_id))
            {
                for (const std::pair<COSEMObjectInstanceID, COSEMObjectValueType>& ObjectValue : m_ObjectValueMap) {
                    if (COSEMObjectInstanceCriteria(ObjectValue.first).Match(Descriptor.instance_id))
                        return true;
                }
            }
        }
        return false;
    }

    ICOSEMAttribute * ICOSEMObject::FindAttribute(ObjectAttributeIdType AttributeId) const
    {
        const ICOSEMInterface * pInterface = dynamic_cast<const ICOSEMInterface *>(this);
        if (pInterface)
        {
        	LOG_ALL("9: FindYYY -> Attribute or Method\r\n");		// ToDo - check breakpoint
            return pInterface->FindAttribute(AttributeId);
        }
        return nullptr;
    }

    ICOSEMMethod * ICOSEMObject::FindMethod(ObjectAttributeIdType MethodId) const
    {
        const ICOSEMInterface * pInterface = dynamic_cast<const ICOSEMInterface *>(this);
        if (pInterface)
        {
            return pInterface->FindMethod(MethodId);
        }
        return nullptr;
    }

    APDUConstants::Data_Access_Result ICOSEMObject::Get(const AssociationContext& Context,
        DLMSVector * pData,
        const Cosem_Attribute_Descriptor& Descriptor,
        SelectiveAccess * pSelectiveAccess /*= nullptr*/)
    {
        LOG_ALL("8: XXX == GET\r\n");		// ToDo - check breakpoint
        APDUConstants::Data_Access_Result RetVal = APDUConstants::Data_Access_Result::object_unavailable;
        ICOSEMAttribute *                 pAttribute = FindAttribute(Descriptor.attribute_id);
        if (pAttribute)
        {
            pAttribute->Clear();
            switch (Descriptor.attribute_id)
            {
            case ICOSEMInterface::ATTRIBUTE_0:
                //
                // UNSUPPORTED
                //
                RetVal = APDUConstants::Data_Access_Result::object_unavailable;
                break;

            case ICOSEMInterface::LOGICAL_NAME:
                if (pAttribute->Append(Descriptor.instance_id))
                {
                    RetVal = APDUConstants::Data_Access_Result::success;
                }
                else
                {
                    RetVal = APDUConstants::Data_Access_Result::temporary_failure;
                }
                break;

            default:
                RetVal = InternalGet(Context, pAttribute, Descriptor, pSelectiveAccess);
                break;
            }
            if (APDUConstants::Data_Access_Result::success == RetVal)
            {
            	LOG_ALL("11: DLMSVector\r\n");		// ToDo - check breakpoint
                pAttribute->GetBytes(pData);
            }
        }
        return RetVal;
    }

    APDUConstants::Data_Access_Result ICOSEMObject::Set(const AssociationContext& Context,
        const Cosem_Attribute_Descriptor& Descriptor,
        const DLMSVector& Data,
        SelectiveAccess * pSelectiveAccess /*= nullptr*/)
    {
        APDUConstants::Data_Access_Result RetVal = APDUConstants::Data_Access_Result::object_unavailable;
        ICOSEMAttribute *                 pAttribute = FindAttribute(Descriptor.attribute_id);
        if (pAttribute)
        {
            pAttribute->Clear();
            switch (Descriptor.attribute_id)
            {
            case ICOSEMInterface::ATTRIBUTE_0:
                //
                // UNSUPPORTED
                //
                RetVal = APDUConstants::Data_Access_Result::object_unavailable;
                break;

            case ICOSEMInterface::LOGICAL_NAME:
                //
                // READ-ONLY
                //
                RetVal = APDUConstants::Data_Access_Result::read_write_denied;
                break;

            default:
                RetVal = InternalSet(Context, pAttribute, Descriptor, Data, pSelectiveAccess);
                break;
            }
        }
        return RetVal;
    }

    APDUConstants::Action_Result ICOSEMObject::Action(const AssociationContext& Context,
        const Cosem_Method_Descriptor& Descriptor,
        const DLMSOptional<DLMSVector>& Parameters,
        DLMSVector * pReturnValue /* = nullptr*/)
    {
        APDUConstants::Action_Result RetVal = APDUConstants::Action_Result::object_unavailable;
        ICOSEMMethod *               pMethod = FindMethod(Descriptor.method_id);
        if (pMethod)
        {
            pMethod->Clear();
            RetVal = InternalAction(Context, pMethod, Descriptor, Parameters, pReturnValue);
        }
        return RetVal;
    }

    APDUConstants::Data_Access_Result ICOSEMObject::InternalSet(const AssociationContext& Context,
        ICOSEMAttribute * pAttribute,
        const Cosem_Attribute_Descriptor& /*Descriptor*/,
        const DLMSVector& Data,
        SelectiveAccess * /*pSelectiveAccess*/)
    {
        if (pAttribute->Parse(Data))
        {
            return APDUConstants::Data_Access_Result::success;
        }
        else
        {
            return APDUConstants::Data_Access_Result::temporary_failure;
        }
    }

    APDUConstants::Action_Result ICOSEMObject::InternalAction(const AssociationContext& Context,
        ICOSEMMethod * pMethod,
        const Cosem_Method_Descriptor& Descriptor,
        const DLMSOptional<DLMSVector>& Parameters,
        DLMSVector * pReturnValue /* = nullptr*/)
    {
        return APDUConstants::Action_Result::object_unavailable;
    }

    APDUConstants::Data_Access_Result ICOSEMObject::InternalAccess(const AssociationContext& Context,
        ICOSEMMethod * pMethod,
        const Cosem_Method_Descriptor& Descriptor,
        const DLMSOptional<DLMSVector>& Parameters,
        DLMSVector * pReturnValue /* = nullptr*/)
    {
        return APDUConstants::Data_Access_Result::object_unavailable;
    }

}
