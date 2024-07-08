
#pragma once

#include "interfaces/IData.h"

namespace EPRI
{
    class LinuxData : public IDataObject
    {
    public:
        LinuxData();
        LinuxData(const COSEMObjectInstanceID& OIDCriteria,
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
    };

} // namespace EPRI
