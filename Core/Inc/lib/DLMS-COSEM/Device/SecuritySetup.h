
#pragma once

#include "interfaces/ISecuritySetup.h"

namespace EPRI
{
    //
    // SecuritySetup
    //
    class SecuritySetup : public ISecuritySetupObject
    {
    public:
        SecuritySetup();
        SecuritySetup(const COSEMObjectInstanceID& OIDCriteria,
            uint16_t ShortNameBase = std::numeric_limits<uint16_t>::max());

    protected:
        virtual APDUConstants::Data_Access_Result InternalGet(const AssociationContext& Context,
            ICOSEMAttribute* pAttribute,
            const Cosem_Attribute_Descriptor& Descriptor,
            SelectiveAccess* pSelectiveAccess) final;
        virtual APDUConstants::Data_Access_Result InternalSet(const AssociationContext& Context,
            ICOSEMAttribute* pAttribute,
            const Cosem_Attribute_Descriptor& Descriptor,
            const DLMSVector& Data,
            SelectiveAccess* pSelectiveAccess) final;
        virtual APDUConstants::Action_Result InternalAction(const AssociationContext& Context,
            ICOSEMMethod* pMethod,
            const Cosem_Method_Descriptor& Descriptor,
            const DLMSOptional<DLMSVector>& Parameters,
            DLMSVector* pReturnValue = nullptr) final;
    };
}