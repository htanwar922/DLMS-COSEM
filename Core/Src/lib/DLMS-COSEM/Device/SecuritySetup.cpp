// Himanshu

#include "Device/SecuritySetup.h"

namespace EPRI {
    SecuritySetup::SecuritySetup()
        : ISecuritySetupObject({ 0, 0, 43, 0, {2, 6}, 255 })
    {
        for (uint8_t i = 2; i <= 6; i++) {
            RegisterObjectInstanceID({ 0, 0, 43, 0, i, 255 });
            
        }
    }

    SecuritySetup::SecuritySetup(const COSEMObjectInstanceID& OIDCriteria, uint16_t ShortNameBase)
        : ISecuritySetupObject(OIDCriteria, ShortNameBase)
    {
    }

    APDUConstants::Data_Access_Result SecuritySetup::InternalGet(const AssociationContext& Context
        , ICOSEMAttribute* pAttribute
        , const Cosem_Attribute_Descriptor& Descriptor
        , SelectiveAccess* pSelectiveAccess)
    {
        return APDUConstants::Data_Access_Result::object_unavailable;
    }

    APDUConstants::Data_Access_Result SecuritySetup::InternalSet(const AssociationContext& Context
        , ICOSEMAttribute* pAttribute
        , const Cosem_Attribute_Descriptor& Descriptor
        , const DLMSVector& Data
        , SelectiveAccess* pSelectiveAccess)
    {
        return APDUConstants::Data_Access_Result::object_unavailable;
    }

    APDUConstants::Action_Result SecuritySetup::InternalAction(const AssociationContext& Context
        , ICOSEMMethod* pMethod
        , const Cosem_Method_Descriptor& Descriptor
        , const DLMSOptional<DLMSVector>& Parameters
        , DLMSVector* pReturnValue)
    {
        return APDUConstants::Action_Result::object_unavailable;
    }
}