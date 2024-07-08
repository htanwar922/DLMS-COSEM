
#include "interfaces/IAssociationLN.h"
#include "Objects/Data.h"

namespace EPRI
{
    //
    // Data
    //
    LinuxData::LinuxData()
        : IDataObject({ 0, 0, 0, 0, 0, 0 })
    {
        // : IDataObject({ 0, 0, 96, 1, {0, 9}, 255 })
        //for (uint8_t i = 0; i <= 9; i++)
        //{
        //    RegisterObjectInstanceID({ 0, 0, 96, 1, i, 255 });
        //    SetAttributeAccessRights({0, 0, 96, 1, i, 255}, LOGICAL_NAME, Association::read_access);
        //    SetAttributeAccessRights({ 0, 0, 96, 1, i, 255 }, ATTR_VALUE, Association::read_access);
        //    
        //    std::string value = "DATA VALUE #" + std::to_string(i);
        //    SetCaptureValue({ 0, 0, 96, 1, i, 255 }, value);
        //}
        for (const MeterConfigType& Config : MeterConfig)
        {
            RegisterObjectInstanceID(Config.OID);
            SetAttributeAccessRights(Config.OID, LOGICAL_NAME, IAssociationLN::read_access);
            SetAttributeAccessRights(Config.OID, ATTR_VALUE, IAssociationLN::read_access);
            SetCaptureValue(Config.OID, Config.Value);
        }
    }

    LinuxData::LinuxData(const COSEMObjectInstanceID& OID,
        uint16_t ShortNameBase /* = std::numeric_limits<uint16_t>::max() */)
        : IDataObject(OID, ShortNameBase)
    {
    }

    APDUConstants::Data_Access_Result LinuxData::InternalGet(const AssociationContext& Context,
        ICOSEMAttribute* pAttribute,
        const Cosem_Attribute_Descriptor& Descriptor,
        SelectiveAccess* pSelectiveAccess)
    {
        LOG_ALL("10: InternalXXX\r\n");		// ToDo - check breakpoint
        pAttribute->SelectChoice(COSEMDataType::VISIBLE_STRING);
        pAttribute->Append(GetCaptureValue(Descriptor.instance_id));
        return APDUConstants::Data_Access_Result::success;
    }

    APDUConstants::Data_Access_Result LinuxData::InternalSet(const AssociationContext& Context,
        ICOSEMAttribute* pAttribute,
        const Cosem_Attribute_Descriptor& Descriptor,
        const DLMSVector& Data,
        SelectiveAccess* pSelectiveAccess)
    {
        APDUConstants::Data_Access_Result RetVal = APDUConstants::Data_Access_Result::temporary_failure;
        try
        {
            DLMSValue Value;

            RetVal = ICOSEMObject::InternalSet(Context, pAttribute, Descriptor, Data, pSelectiveAccess);
            if (APDUConstants::Data_Access_Result::success == RetVal &&
                pAttribute->GetNextValue(&Value) == COSEMType::GetNextResult::VALUE_RETRIEVED)
            {
                SetCaptureValue(Descriptor.instance_id, Value);
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

} // namespace EPRI