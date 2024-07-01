#pragma once

#include "interfaces/ICOSEMInterface.h"

namespace EPRI
{
    const ClassIDType CLSID_IRegister = 3;

    class IRegister_0 : public ICOSEMInterface
    {
        COSEM_DEFINE_SCHEMA(Value_Schema)
        COSEM_DEFINE_SCHEMA(Scalar_Unit_Schema)
    public:

        IRegister_0();
        virtual ~IRegister_0();

        enum Attributes : ObjectAttributeIdType
        {
            ATTR_VALUE = 2,
            ATTR_SCALAR_UNIT = 3,
        };

        COSEMAttribute<ATTR_VALUE, Value_Schema, 0x08> value;
        COSEMAttribute<ATTR_SCALAR_UNIT, Scalar_Unit_Schema, 0x10> scalar_unit_type;

        enum Methods : ObjectAttributeIdType
        {
            METHOD_RESET = 1
        };

        COSEMMethod<METHOD_RESET, IntegerSchema, 0x28> reset_data;  // Himanshu

        // Himanshu - Added to get access rights for each attribute and method
        uint8_t GetAttributeAccessRights(ObjectAttributeIdType AttributeId) const;
        uint8_t GetMethodAccessRights(ObjectAttributeIdType MethodId) const;
        DLMSStructure GetAccessRights() const;
    };

    typedef IRegister_0 IRegister;

    class IRegisterObject : public IRegister_0, public ICOSEMObject
    {
    public:
    	IRegisterObject() = delete;
        IRegisterObject(const COSEMObjectInstanceCriteria& OIDCriteria,
            uint16_t ShortNameBase = std::numeric_limits<uint16_t>::max());
        virtual ~IRegisterObject();

    };

}
