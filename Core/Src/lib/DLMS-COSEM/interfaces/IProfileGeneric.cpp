#include "interfaces/IProfileGeneric.h"
#include "interfaces/IAssociationLN.h"

namespace EPRI
{
    COSEM_BEGIN_SCHEMA(IProfileGeneric_1::Buffer_Schema)
        COSEM_BEGIN_ARRAY
            COSEM_BEGIN_STRUCTURE
                COSEM_BEGIN_CHOICE
                    COSEM_NULL_DATA_TYPE
                    // COSEM_ARRAY_TYPE
                    // COSEM_STRUCTURE_TYPE
                    COSEM_BOOLEAN_TYPE
                    COSEM_BIT_STRING_TYPE(24)
                    COSEM_DOUBLE_LONG_TYPE
                    COSEM_DOUBLE_LONG_UNSIGNED_TYPE
                    COSEM_FLOATING_POINT_TYPE
                    COSEM_OCTET_STRING_TYPE
                    COSEM_VISIBLE_STRING_TYPE
                    COSEM_UTF8_STRING_TYPE
                    COSEM_BCD_TYPE
                    COSEM_INTEGER_TYPE
                    COSEM_LONG_TYPE
                    COSEM_UNSIGNED_TYPE
                    COSEM_LONG_UNSIGNED_TYPE
                    COSEM_LONG64_TYPE
                    COSEM_LONG64_UNSIGNED_TYPE
                    COSEM_ENUM_TYPE
                    (
                        {
                            // Add Enums here
                        }
                    )
                    COSEM_FLOAT32_TYPE
                    COSEM_FLOAT64_TYPE
                    COSEM_DATE_TIME_TYPE
                    COSEM_DATE_TYPE
                    COSEM_TIME_TYPE
                COSEM_END_CHOICE
            COSEM_END_STRUCTURE
        COSEM_END_ARRAY
    COSEM_END_SCHEMA

    COSEM_BEGIN_SCHEMA(IProfileGeneric_1::Capture_Objects_Schema)
        COSEM_BEGIN_ARRAY
            COSEM_BEGIN_STRUCTURE
                COSEM_LONG_UNSIGNED_TYPE    // Class ID
                COSEM_OCTET_STRING_TYPE     // Logical Name
                COSEM_INTEGER_TYPE          // Attribute Index
                COSEM_LONG_UNSIGNED_TYPE    // Data Index
            COSEM_END_STRUCTURE
        COSEM_END_ARRAY
    COSEM_END_SCHEMA

    COSEM_BEGIN_SCHEMA(IProfileGeneric_1::Sort_Method_Schema)
        COSEM_ENUM_TYPE
        (
            {
                IProfileGeneric_1::SORT_METHOD_FIFO,
                IProfileGeneric_1::SORT_METHOD_LIFO,
                IProfileGeneric_1::SORT_METHOD_LARGEST,
                IProfileGeneric_1::SORT_METHOD_SMALLEST,
                IProfileGeneric_1::SORT_METHOD_NEAREST_TO_ZERO,
                IProfileGeneric_1::SORT_METHOD_FAREST_FROM_ZERO
            }
        )
    COSEM_END_SCHEMA

    COSEM_BEGIN_SCHEMA(IProfileGeneric_1::Sort_Object_Schema)
        COSEM_BEGIN_STRUCTURE
            COSEM_LONG_UNSIGNED_TYPE    // Class ID
            COSEM_OCTET_STRING_TYPE     // Logical Name
            COSEM_INTEGER_TYPE          // Attribute Index
            COSEM_LONG_UNSIGNED_TYPE    // Data Index
        COSEM_END_STRUCTURE
    COSEM_END_SCHEMA

    //
    // IProfileGeneric_1
    //
    IProfileGeneric_1::IProfileGeneric_1() : ICOSEMInterface(CLSID_IProfileGeneric, 0)
    {
        COSEM_BEGIN_ATTRIBUTES
            COSEM_ATTRIBUTE(logical_name)
            COSEM_ATTRIBUTE(buffer)
            COSEM_ATTRIBUTE(capture_objects)
            COSEM_ATTRIBUTE(capture_period)
            COSEM_ATTRIBUTE(sort_method)
            COSEM_ATTRIBUTE(sort_object)
            COSEM_ATTRIBUTE(entries_in_use)
            COSEM_ATTRIBUTE(profile_entries)
        COSEM_END_ATTRIBUTES

        COSEM_BEGIN_METHODS
            COSEM_METHOD(reset_data)
            COSEM_METHOD(capture_data)
        COSEM_END_METHODS
    }

    IProfileGeneric_1::~IProfileGeneric_1()
    {
    }

    // Himanshu
    uint8_t IProfileGeneric_1::GetAttributeAccessRights(ObjectAttributeIdType AttributeId) const
    {
        int8_t access = IAssociationLN::access_denied;
        switch (AttributeId)
        {
        case LOGICAL_NAME:
            access |= IAssociationLN::read_access;
            break;
        case ATTR_BUFFER:
            access |= IAssociationLN::read_access;
            break;
        case ATTR_CAPTURE_OBJECTS:
            access |= IAssociationLN::read_access;
            break;
        case ATTR_CAPTURE_PERIOD:
            access |= IAssociationLN::read_access;
            break;
        case ATTR_SORT_METHOD:
            access |= IAssociationLN::read_access;
            break;
        case ATTR_SORT_OBJECT:
            access |= IAssociationLN::read_access;
            break;
        case ATTR_ENTRIES_IN_USE:
            access |= IAssociationLN::read_access;
            break;
        case ATTR_PROFILE_ENTRIES:
            access |= IAssociationLN::read_access;
            break;
        }
        return access;
    }

    // Himanshu
    uint8_t IProfileGeneric_1::GetMethodAccessRights(ObjectAttributeIdType MethodId) const
    {
        int8_t access = IAssociationLN::access_denied;
        switch (MethodId)
        {
        case METHOD_RESET:
            access |= IAssociationLN::authenticated_request
                | IAssociationLN::encrypted_request
                | IAssociationLN::digitally_signed_request;
            break;
        case METHOD_CAPTURE:
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
    DLMSStructure IProfileGeneric_1::GetAccessRights() const
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
    // IProfileGenericObject
    //
    IProfileGenericObject::IProfileGenericObject(const COSEMObjectInstanceCriteria& OIDCriteria,
                             uint16_t ShortNameBase /* = std::numeric_limits<uint16_t>::max() */) :
        ICOSEMObject(OIDCriteria, ShortNameBase)
    {
    }

    IProfileGenericObject::~IProfileGenericObject()
    {
    }
}
