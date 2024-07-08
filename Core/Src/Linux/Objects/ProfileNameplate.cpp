
#include "interfaces/IAssociationLN.h"
#include "Objects/ProfileNameplate.h"
#include "Objects/Data.h"

namespace EPRI
{
    //
    // ProfileGeneric // 0-0:94.91.10*255
    //
    LinuxProfileNameplate::LinuxProfileNameplate(const std::vector<ICOSEMObject*>& Objects)
        : IProfileGenericObject({ 0, 0, 94, 91, 10, 255 })
        , m_Objects(Objects)
    {
        RegisterObjectInstanceID({ 0, 0, 94, 91, 10, 255 });
        SetCaptureObjects({
            CaptureObjectType{  CLSID_IData,    {   0,   0,  96,   1,   0, 255 },      2,      0    },
            CaptureObjectType{  CLSID_IData,    {   0,   0,  96,   1,   1, 255 },      2,      0    },
            CaptureObjectType{  CLSID_IData,    {   0,   0,  96,   1,   2, 255 },      2,      0    },
            CaptureObjectType{  CLSID_IData,    {   1,   0,   0,   2,   0, 255 },      2,      0    },
            CaptureObjectType{  CLSID_IData,    {   0,   0,  94,  91,   9, 255 },      2,      0    },
            CaptureObjectType{  CLSID_IData,    {   0,   0,  94,  91,  11, 255 },      2,      0    },
            CaptureObjectType{  CLSID_IData,    {   0,   0,  94,  91,  12, 255 },      2,      0    },
            CaptureObjectType{  CLSID_IData,    {   0,   0,  96,   1,   4, 255 },      2,      0    },
            });
        SetCapturePeriod(0);
        SetSortObject({ CLSID_IData, { 0, 0, 96, 1, 0, 255 }, 2 });
        SetSortMethod(SORT_METHOD_FIFO);
        SetProfileEntries(1);
        SetEntriesInUse(1);
        SetBuffer({});

        SetAttributeAccessRights(m_InstanceCriteria, ATTR_BUFFER, IAssociationLN::read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_CAPTURE_OBJECTS, IAssociationLN::read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_CAPTURE_PERIOD, IAssociationLN::read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_ENTRIES_IN_USE, IAssociationLN::read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_PROFILE_ENTRIES, IAssociationLN::read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_SORT_METHOD, IAssociationLN::read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_SORT_OBJECT, IAssociationLN::read_access);
        SetMethodAccessRights(m_InstanceCriteria, METHOD_RESET, IAssociationLN::write_access);
        SetMethodAccessRights(m_InstanceCriteria, METHOD_CAPTURE, IAssociationLN::write_access);
    }

    void LinuxProfileNameplate::CaptureData()
    {
        DLMSStructure Data;
        for (const CaptureObjectType& Object : m_CaptureObjects)
        {
            for (const ICOSEMObject* pObject : m_Objects)
            {
                const ICOSEMInterface* pInterface = dynamic_cast<const ICOSEMInterface*>(pObject);
                if (pObject->GetClassID() == Object.object.class_id)
                {
                    if (2 == Object.object.attribute_id)
                        Data.push_back(pObject->GetCaptureValue(Object.object.instance_id));
                    else {
                        //AssociationContext Context; //DLMSVector Data; //pObject->Get(Context, &Data, Object.object);
                        throw std::runtime_error("Not expected");
                    }
                }
            }
        }
        m_Buffer.push_back(Data);
    }

    APDUConstants::Data_Access_Result LinuxProfileNameplate::InternalGet(const AssociationContext& Context,
        ICOSEMAttribute* pAttribute,
        const Cosem_Attribute_Descriptor& Descriptor,
        SelectiveAccess* pSelectiveAccess)
    {
        switch (Descriptor.attribute_id)
        {
        case ATTR_BUFFER: {
            pAttribute->SelectChoice(COSEMDataType::VISIBLE_STRING);
            DLMSArray Buffer;
            for (auto& Value : m_Buffer)
            {
                Buffer.push_back(Value);
                //Buffer.push_back(DLMSStructure{std::string("X")});
            }
            pAttribute->Append(Buffer);
            break;
        }
        case ATTR_CAPTURE_OBJECTS: {
            DLMSArray CaptureObjects;
            for (CaptureObjectType& Object : m_CaptureObjects)
            {
                CaptureObjects.push_back(DLMSStructure(Object));
            }
            pAttribute->Append(CaptureObjects);
            break;
        }
        case ATTR_CAPTURE_PERIOD: {
            pAttribute->Append(m_CapturePeriod);
            break;
        }
        case ATTR_ENTRIES_IN_USE: {
            pAttribute->Append(m_EntriesInUse);
            break;
        }
        case ATTR_PROFILE_ENTRIES: {
            pAttribute->Append(m_ProfileEntries);
            break;
        }
        case ATTR_SORT_METHOD: {
            pAttribute->SelectChoice(COSEMDataType::ENUM);
            pAttribute->Append(m_SortMethod);
            break;
        }
        case ATTR_SORT_OBJECT: {
            pAttribute->Append(DLMSStructure(*m_pSortObject));
            break;
        }
        default:
            LOG_ERROR("Unknown attribute called\r\n");
            return APDUConstants::Data_Access_Result::object_unavailable;
        }
        LOG_DEBUG("At the end of LinuxProfileNameplate::InternalGet\r\n");

        return APDUConstants::Data_Access_Result::success;
    }

    APDUConstants::Data_Access_Result LinuxProfileNameplate::InternalSet(const AssociationContext& Context,
        ICOSEMAttribute* pAttribute,
        const Cosem_Attribute_Descriptor& Descriptor,
        const DLMSVector& Data,
        SelectiveAccess* pSelectiveAccess)
    {
        APDUConstants::Data_Access_Result RetVal = APDUConstants::Data_Access_Result::temporary_failure;
        try
        {
            DLMSValue Value;

            if (not (GetAttributeAccessRights(Descriptor.instance_id, Descriptor.attribute_id) & IAssociationLN::write_access))
            {
                return APDUConstants::Data_Access_Result::scope_of_access_violated;
            }

            RetVal = ICOSEMObject::InternalSet(Context, pAttribute, Descriptor, Data, pSelectiveAccess);
            if (APDUConstants::Data_Access_Result::success == RetVal &&
                pAttribute->GetNextValue(&Value) == COSEMType::GetNextResult::VALUE_RETRIEVED)
            {
                switch (Descriptor.attribute_id)
                {
                default:
                    break;
                }
                RetVal = APDUConstants::Data_Access_Result::success;
            }
            else
            {
                RetVal = APDUConstants::Data_Access_Result::type_unmatched;
            }
        }
        catch (...)
        {
            RetVal = APDUConstants::Data_Access_Result::type_unmatched;
        }
        return RetVal;
    }

    APDUConstants::Action_Result LinuxProfileNameplate::InternalAction(const AssociationContext& Context,
        ICOSEMMethod* pMethod,
        const Cosem_Method_Descriptor& Descriptor,
        const DLMSOptional<DLMSVector>& Parameters,
        DLMSVector* pReturnValue /*= nullptr*/)
    {
        APDUConstants::Action_Result RetVal = APDUConstants::Action_Result::temporary_failure;
        try
        {
            switch (Descriptor.method_id)
            {
            case METHOD_RESET:
                ResetData();
                RetVal = APDUConstants::Action_Result::success;
                break;
            case METHOD_CAPTURE:
                if (Parameters)
                {
                    CaptureData();
                    RetVal = APDUConstants::Action_Result::success;
                }
                else
                {
                    RetVal = APDUConstants::Action_Result::other_reason;
                }
                break;
            default:
                RetVal = APDUConstants::Action_Result::object_unavailable;
                break;
            }
        }
        catch (...)
        {
            RetVal = APDUConstants::Action_Result::temporary_failure;
        }
        return RetVal;
    }
} // namespace EPRI