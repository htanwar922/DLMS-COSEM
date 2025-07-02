
#include "interfaces/IAssociationLN.h"
#include "Objects/ProfileBlockLoad.h"
#include "Objects/Clock.h"
#include "Objects/Register.h"
#include "BaseLibrary.h"

extern EPRI::LinuxBaseLibrary g_BL;

namespace EPRI
{
/*

*/

    //
    // ProfileGeneric // 1-0:99.1.0*255
    //
    LinuxProfileBlockLoad::LinuxProfileBlockLoad(const std::vector<ICOSEMObject*>& Objects)
        : IProfileGenericObject({ 1, 0, 99, 1, 0, 255 })
        , m_Objects(Objects)
        , m_pCaptureTimer(Base()->GetCore()->CreateSimpleTimer())
    {
        RegisterObjectInstanceID({ 1, 0, 99, 1, 0, 255 });
        SetCaptureObjects({
            CaptureObjectType{  CLSID_IClock,   	{  0,   0,   1,   0,   0, 255 },      2,      0    },
            CaptureObjectType{  CLSID_IRegister,    {  1,   0,  12,  27,   0, 255 },      2,      0    },
            CaptureObjectType{  CLSID_IRegister,    {  1,   0,   1,  29,   0, 255 },      2,      0    },
            CaptureObjectType{  CLSID_IRegister,    {  1,   0,   9,  29,   0, 255 },      2,      0    },
            CaptureObjectType{  CLSID_IRegister,    {  1,   0,   2,  29,   0, 255 },      2,      0    },
            CaptureObjectType{  CLSID_IRegister,    {  1,   0,  10,  29,   0, 255 },      2,      0    },
            CaptureObjectType{  CLSID_IRegister,    {  1,   0,  11,  27,   0, 255 },      2,      0    },
            CaptureObjectType{  CLSID_IRegister,    {  0,   0,  96,  10,   1, 255 },      2,      0    },
            CaptureObjectType{  CLSID_IRegister,    {  0,   1,  96,  12,   5, 255 },      2,      0    },
        });

        SetSortObject({ CLSID_IClock, { 0, 0, 1, 0, 0, 255 }, 2 });
        SetSortMethod(SORT_METHOD_FIFO);
        SetProfileEntries(100);
        SetEntriesInUse(0);
        SetBuffer({});

        SetAttributeAccessRights(m_InstanceCriteria, ATTR_BUFFER, IAssociationLN::attr_read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_CAPTURE_OBJECTS, IAssociationLN::attr_read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_CAPTURE_PERIOD, IAssociationLN::attr_read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_ENTRIES_IN_USE, IAssociationLN::attr_read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_PROFILE_ENTRIES, IAssociationLN::attr_read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_SORT_METHOD, IAssociationLN::attr_read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_SORT_OBJECT, IAssociationLN::attr_read_access);
        SetMethodAccessRights(m_InstanceCriteria, METHOD_RESET, IAssociationLN::method_access);
        SetMethodAccessRights(m_InstanceCriteria, METHOD_CAPTURE, IAssociationLN::method_access);

        SetCapturePeriod(30);
        m_pCaptureTimer->SetCallback(std::bind(&LinuxProfileBlockLoad::CaptureData, this), true);
        m_pCaptureTimer->Initialize(m_CapturePeriod * 1000);
        m_pCaptureTimer->TriggerCallback();
    }

    void LinuxProfileBlockLoad::CaptureData()
    {
        m_pCaptureTimer->Initialize(m_CapturePeriod * 1000);

        LOG_INFO("Capturing Block Load data\r\n");
        DLMSStructure Data;
        for (const CaptureObjectType& Object : m_CaptureObjects)
        {
            for (const ICOSEMObject* pObject : m_Objects)
            {
                const ICOSEMInterface* pInterface = dynamic_cast<const ICOSEMInterface*>(pObject);
                if (pObject->GetClassID() == Object.object.class_id and pObject->Supports(Object.object))
                {
                    if (Object.object.class_id == CLSID_IClock)
                    {
                        const LinuxClock* pClock = dynamic_cast<const LinuxClock*>(pObject);
                        if (pClock)
                        {
                            Data.push_back(pClock->GetCaptureValue(Object.object));
                            break;
                        }
                        break;
                    }
                    Data.push_back(pObject->GetCaptureValue(Object.object));
                    break;
                }
            }
        }
        if (Data.size() == m_CaptureObjects.size())
        {
            m_Buffer.push_back(Data);
            m_EntriesInUse++;
            if (m_EntriesInUse > m_ProfileEntries)
            {
                m_Buffer.erase(m_Buffer.begin());
                m_EntriesInUse--;
            }
            LOG_INFO("Captured Block Load data\r\n");
        }
        else
        {
            LOG_ERROR("Failed to capture Block Load data\r\n");
        }
    }

    APDUConstants::Data_Access_Result LinuxProfileBlockLoad::InternalGet(const AssociationContext& Context,
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
        LOG_DEBUG("At the end of LinuxProfileBlockLoad::InternalGet\r\n");

        return APDUConstants::Data_Access_Result::success;
    }

    APDUConstants::Data_Access_Result LinuxProfileBlockLoad::InternalSet(const AssociationContext& Context,
        ICOSEMAttribute* pAttribute,
        const Cosem_Attribute_Descriptor& Descriptor,
        const DLMSVector& Data,
        SelectiveAccess* pSelectiveAccess)
    {
        APDUConstants::Data_Access_Result RetVal = APDUConstants::Data_Access_Result::temporary_failure;
        try
        {
            DLMSValue Value;

            if (not (GetAttributeAccessRights(Descriptor.instance_id, Descriptor.attribute_id) & IAssociationLN::attr_write_access))
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

    APDUConstants::Action_Result LinuxProfileBlockLoad::InternalAction(const AssociationContext& Context,
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