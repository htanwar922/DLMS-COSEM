
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
            SetAttributeAccessRights(m_InstanceCriteria, Attribute.first, IAssociationLN::read_access);
        for (const std::pair<ObjectMethodIdType, ICOSEMMethod*>& Method : m_Methods)
            SetMethodAccessRights(m_InstanceCriteria, Method.first, IAssociationLN::write_access);
    }

    APDUConstants::Data_Access_Result LinuxClock::InternalGet(const AssociationContext& Context,
        ICOSEMAttribute* pAttribute,
        const Cosem_Attribute_Descriptor& Descriptor,
        SelectiveAccess* pSelectiveAccess)
    {
        COSEMDateTime Now(true);
        switch (pAttribute->AttributeID)
        {
        case ATTR_TIME:
            pAttribute->Append(Now);
            break;
        case ATTR_TIME_ZONE:
            pAttribute->Append(Now.GetTimeZone());
            break;
        case ATTR_STATUS:
            pAttribute->Append(Now.GetClockStatus());
            break;
        case ATTR_DST_BEGIN:
            pAttribute->Append(Now.GetDaylightSavingsBegin());
            break;
        case ATTR_DST_END:
            pAttribute->Append(Now.GetDaylightSavingsEnd());
            break;
        case ATTR_DST_DEVIATION:
            pAttribute->Append(Now.GetDaylightSavingsDeviation());
            break;
        case ATTR_DST_ENABLED:
            pAttribute->Append(false);
            break;
        case ATTR_CLOCK_BASE:
            pAttribute->Append(Now.GetClockBase());
            break;
        }
        //
        // TODO
        //
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

} // namespace EPRI