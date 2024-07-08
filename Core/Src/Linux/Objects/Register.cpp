
#include "interfaces/IAssociationLN.h"
#include "Objects/Register.h"

namespace EPRI
{
    //
    // Register // Sudeshna //1-0:12.7.0*255
    //
    LinuxRegister::LinuxRegister()
        : IRegisterObject({ 1, 0, 0, 7, 0, 255 })
    {
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_VALUE, IAssociationLN::read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_SCALAR_UNIT, IAssociationLN::read_access);
        SetMethodAccessRights(m_InstanceCriteria, METHOD_RESET, IAssociationLN::write_access);

        SetCaptureValue(m_InstanceCriteria, 0.f);
    }

    LinuxRegister::LinuxRegister(const COSEMObjectInstanceID& OID,
        uint16_t ShortNameBase /* = std::numeric_limits<uint16_t>::max() */)
        : IRegisterObject(OID, ShortNameBase)
    {
    }

    APDUConstants::Data_Access_Result LinuxRegister::InternalGet(const AssociationContext& Context,
        ICOSEMAttribute* pAttribute,
        const Cosem_Attribute_Descriptor& Descriptor,
        SelectiveAccess* pSelectiveAccess)
    {
        // Himanshu
        uint8_t obis_vg_c = Descriptor.instance_id.GetValueGroup(EPRI::COSEMObjectInstanceID::VALUE_GROUP_C);
        LOG_NOTICE("LinuxRegister::InternalGet : OBIS : %d\r\n", obis_vg_c);

        switch (Descriptor.attribute_id)
        {
        case ATTR_VALUE: {
            float value = GetCaptureValue(Descriptor.instance_id);
            pAttribute->SelectChoice(COSEMDataType::FLOAT32);
            pAttribute->Append(value);
            break;
        }
        case (ObjectAttributeIdType)ATTR_SCALAR_UNIT: {
            pAttribute->Append(DLMSStructure{
                (int8_t)0,
                (uint8_t)0
                });
            break;
        }
        default:
            LOG_ERROR("Unknown attribute called\r\n");
            return APDUConstants::Data_Access_Result::object_unavailable;
        }
        LOG_DEBUG("At the end of LinuxRegister::InternalGet\r\n");

        return APDUConstants::Data_Access_Result::success;
    }

    APDUConstants::Data_Access_Result LinuxRegister::InternalSet(const AssociationContext& Context,
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

    APDUConstants::Action_Result LinuxRegister::InternalAction(const AssociationContext& Context,
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
                SetCaptureValue(m_InstanceCriteria, 0.f);
                RetVal = APDUConstants::Action_Result::success;
                break;
            default:
                RetVal = APDUConstants::Action_Result::object_unavailable;
                break;
            }
        }
        catch (...)
        {
            RetVal = APDUConstants::Action_Result::object_unavailable;
        }
        return RetVal;
    }

} // namespace EPRI