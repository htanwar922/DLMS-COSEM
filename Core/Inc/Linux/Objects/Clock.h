
#pragma once

#include "interfaces/IClock.h"

namespace EPRI
{

    class LinuxClock : public IClockObject
    {
    public:
        LinuxClock();

    protected:
        virtual APDUConstants::Data_Access_Result InternalGet(const AssociationContext& Context,
            ICOSEMAttribute* pAttribute,
            const Cosem_Attribute_Descriptor& Descriptor,
            SelectiveAccess* pSelectiveAccess) final;
        virtual APDUConstants::Action_Result InternalAction(const AssociationContext& Context,
            ICOSEMMethod* pMethod,
            const Cosem_Method_Descriptor& Descriptor,
            const DLMSOptional<DLMSVector>& Parameters,
            DLMSVector* pReturnValue = nullptr) final;
    };

} // namespace EPRI
