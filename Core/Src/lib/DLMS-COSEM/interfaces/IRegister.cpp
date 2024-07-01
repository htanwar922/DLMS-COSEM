#include "interfaces/IRegister.h"
#include "interfaces/IAssociationLN.h"

namespace EPRI
{

    COSEM_BEGIN_SCHEMA(IRegister_0::Value_Schema)
        COSEM_BEGIN_CHOICE
            COSEM_NULL_DATA_TYPE
            COSEM_BIT_STRING_TYPE(24)
            COSEM_DOUBLE_LONG_TYPE
            COSEM_DOUBLE_LONG_UNSIGNED_TYPE
            COSEM_OCTET_STRING_TYPE
            COSEM_VISIBLE_STRING_TYPE
            COSEM_UTF8_STRING_TYPE
            COSEM_INTEGER_TYPE
            COSEM_LONG_TYPE
            COSEM_UNSIGNED_TYPE
            COSEM_LONG_UNSIGNED_TYPE
            COSEM_LONG64_TYPE
            COSEM_LONG64_UNSIGNED_TYPE
            COSEM_FLOAT32_TYPE
            COSEM_FLOAT64_TYPE
        COSEM_END_CHOICE
    COSEM_END_SCHEMA

    COSEM_BEGIN_SCHEMA(IRegister_0::Scalar_Unit_Schema)
        // Himanshu - modified
        COSEM_BEGIN_STRUCTURE
            COSEM_INTEGER_TYPE      // scalar - exponent to base 10, 0 if value is non-numerical
            COSEM_ENUM_TYPE         // unit - enum - see Table 4. IEC 62056-6-2:2023 Page 56+
            (
                {
                    // Add Units here
                }
            )
        COSEM_END_STRUCTURE
    COSEM_END_SCHEMA

    //
    // IRegister_0
    //
    IRegister_0::IRegister_0() : ICOSEMInterface(CLSID_IRegister, 0)
    {
        COSEM_BEGIN_ATTRIBUTES
            COSEM_ATTRIBUTE(value)
            COSEM_ATTRIBUTE(scalar_unit_type)
        COSEM_END_ATTRIBUTES

        COSEM_BEGIN_METHODS
            COSEM_METHOD(reset_data)                // Himanshu
        COSEM_END_METHODS
    }

    IRegister_0::~IRegister_0()
    {
    }

    // Himanshu
    uint8_t IRegister_0::GetAttributeAccessRights(ObjectAttributeIdType AttributeId) const
    {
        int8_t access = IAssociationLN::access_denied;
        switch (AttributeId)
        {
        case LOGICAL_NAME:
            access |= IAssociationLN::read_access;
            break;
        case ATTR_VALUE:
            access |= IAssociationLN::read_access;
            break;
        case ATTR_SCALAR_UNIT:
            access |= IAssociationLN::read_access;
            break;
        }
        return access;
    }

    // Himanshu
    uint8_t IRegister_0::GetMethodAccessRights(ObjectAttributeIdType MethodId) const
    {
        int8_t access = IAssociationLN::access_denied;
        switch (MethodId)
        {
        case METHOD_RESET:
            access |= IAssociationLN::authenticated_request
                | IAssociationLN::encrypted_request
                | IAssociationLN::digitally_signed_request;
            break;
        default:
            break;
        }
        return access;
    }

    // Himanshu
    DLMSStructure IRegister_0::GetAccessRights() const
    {
        DLMSStructure data;
        DLMSArray array;
        for(COSEMAttributeMap::value_type attribute : m_Attributes)
        {
            array.push_back(DLMSStructure({
                (int8_t)attribute.first,
                (uint8_t)GetAttributeAccessRights(attribute.first),
                DLMSBlank
            }));
        }
        data.push_back(array);

        array.clear();
        for (COSEMMethodMap::value_type method : m_Methods)
        {
            array.push_back(DLMSStructure({
                (int8_t)method.first,
                (uint8_t)GetMethodAccessRights(method.first)
            }));
        }
        data.push_back(array);

        return data;
    }
    //
    // IRegisterObject
    //
    IRegisterObject::IRegisterObject(const COSEMObjectInstanceCriteria& OIDCriteria,
                             uint16_t ShortNameBase /* = std::numeric_limits<uint16_t>::max() */) :
        ICOSEMObject(OIDCriteria, ShortNameBase)
    {
    }

    IRegisterObject::~IRegisterObject()
    {
    }
}
