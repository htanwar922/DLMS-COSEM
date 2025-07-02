
#include "interfaces/IAssociationLN.h"
#include "Objects/Clock.h"
#include "DateTime.h"

namespace EPRI
{
    //
    // Clock
    //
    LinuxClock::LinuxClock()
        : IClockObject({ 0, 0, 1, 0, 0, 255 })
    {
        for (const std::pair<ObjectAttributeIdType, ICOSEMAttribute*>& Attribute : m_Attributes)
            SetAttributeAccessRights(m_InstanceCriteria, Attribute.first, IAssociationLN::attr_read_access);
        for (const std::pair<ObjectMethodIdType, ICOSEMMethod*>& Method : m_Methods)
            SetMethodAccessRights(m_InstanceCriteria, Method.first, IAssociationLN::method_access);
    }

    APDUConstants::Data_Access_Result LinuxClock::InternalGet(const AssociationContext& Context,
        ICOSEMAttribute* pAttribute,
        const Cosem_Attribute_Descriptor& Descriptor,
        SelectiveAccess* pSelectiveAccess)
    {
        if (not pAttribute->Append(GetCaptureValue(Descriptor)))
            return APDUConstants::Data_Access_Result::temporary_failure;
        return APDUConstants::Data_Access_Result::success;
    }

    APDUConstants::Action_Result LinuxClock::InternalAction(const AssociationContext& Context,
        ICOSEMMethod* pMethod,
        const Cosem_Method_Descriptor& Descriptor,
        const DLMSOptional<DLMSVector>& Parameters,
        DLMSVector* pReturnValue /*= nullptr*/)
    {
        switch (pMethod->MethodID)
        {
        case METHOD_ADJUST_TO_QUARTER:
        case METHOD_ADJUST_TO_MEAS_PERIOD:
        case METHOD_ADJUST_TO_MINUTE:
        case METHOD_ADJUST_TO_PRESET_TIME:
        case METHOD_PRESET_ADJUSTING_TIME:
        case METHOD_SHIFT_TIME:
        default:
            std::cout << "Clock ACTION Received" << std::endl;
            break;
        }
        //
        // TODO
        //
        return APDUConstants::Action_Result::object_unavailable;
    }

    DLMSValue LinuxClock::GetCaptureValue(const EPRI::Cosem_Attribute_Descriptor& OBIS) const
    {
        COSEMDateTime Now(true);
        if (OBIS.class_id == CLSID_IClock and OBIS.instance_id == m_InstanceCriteria) {
            switch (OBIS.attribute_id)
            {
            case ATTR_TIME:
                return Now;
            case ATTR_TIME_ZONE:
                return Now.GetTimeZone();
            case ATTR_STATUS:
                return Now.GetClockStatus();
            case ATTR_DST_BEGIN:
                return Now.GetDaylightSavingsBegin();
            case ATTR_DST_END:
                return Now.GetDaylightSavingsEnd();
            case ATTR_DST_DEVIATION:
                return Now.GetDaylightSavingsDeviation();
            case ATTR_DST_ENABLED:
                return false;
            case ATTR_CLOCK_BASE:
                return Now.GetClockBase();
            }
        }
        return DLMSBlank;
    }

} // namespace EPRI