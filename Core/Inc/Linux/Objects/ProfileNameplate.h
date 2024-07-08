
#pragma once

#include "interfaces/IProfileGeneric.h"

namespace EPRI
{
    //
    // LinuxProfileNameplate
    //
    class LinuxProfileNameplate : public IProfileGenericObject
    {
    public:
        LinuxProfileNameplate(const std::vector<ICOSEMObject*>& Objects);
        virtual void CaptureData() final;

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
    private:
        const std::vector<ICOSEMObject*>& m_Objects;
    };

} // namespace EPRI
