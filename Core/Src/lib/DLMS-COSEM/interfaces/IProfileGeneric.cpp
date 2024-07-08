#include "interfaces/IProfileGeneric.h"
#include "interfaces/IAssociationLN.h"

namespace EPRI
{
#define CAPTURE_OBJECT_DEFINITION                               \
        COSEM_BEGIN_STRUCTURE                                   \
            COSEM_LONG_UNSIGNED_TYPE    /* Class ID         */  \
            COSEM_OCTET_STRING_TYPE     /* Logical Name     */  \
            COSEM_INTEGER_TYPE          /* Attribute Index  */  \
            COSEM_LONG_UNSIGNED_TYPE    /* Data Index       */  \
        COSEM_END_STRUCTURE

#define RANGE_VALUE_CHOICES             \
        COSEM_DOUBLE_LONG_TYPE          \
        COSEM_DOUBLE_LONG_UNSIGNED_TYPE \
        COSEM_OCTET_STRING_TYPE         \
        COSEM_VISIBLE_STRING_TYPE       \
        COSEM_UTF8_STRING_TYPE          \
        COSEM_INTEGER_TYPE              \
        COSEM_LONG_TYPE                 \
        COSEM_UNSIGNED_TYPE             \
        COSEM_LONG_UNSIGNED_TYPE        \
        COSEM_LONG64_TYPE               \
        COSEM_LONG64_UNSIGNED_TYPE      \
        COSEM_FLOAT32_TYPE              \
        COSEM_FLOAT64_TYPE              \
        COSEM_DATE_TIME_TYPE            \
        COSEM_DATE_TYPE                 \
        COSEM_TIME_TYPE

    COSEM_BEGIN_SCHEMA(IProfileGeneric_1::Buffer_Schema)
        COSEM_BEGIN_ARRAY
            COSEM_BEGIN_STRUCTURE
                COSEM_ANY_ELEMENT_TYPE
                //COSEM_BEGIN_CHOICE
                //    COSEM_NULL_DATA_TYPE
                //    COSEM_BOOLEAN_TYPE
                //    COSEM_BIT_STRING_TYPE(24)
                //    COSEM_DOUBLE_LONG_TYPE
                //    COSEM_DOUBLE_LONG_UNSIGNED_TYPE
                //    COSEM_FLOATING_POINT_TYPE
                //    COSEM_OCTET_STRING_TYPE
                //    COSEM_VISIBLE_STRING_TYPE
                //    COSEM_UTF8_STRING_TYPE
                //    COSEM_BCD_TYPE
                //    COSEM_INTEGER_TYPE
                //    COSEM_LONG_TYPE
                //    COSEM_UNSIGNED_TYPE
                //    COSEM_LONG_UNSIGNED_TYPE
                //    COSEM_LONG64_TYPE
                //    COSEM_LONG64_UNSIGNED_TYPE
                //    COSEM_ENUM_TYPE({})
                //    COSEM_FLOAT32_TYPE
                //    COSEM_FLOAT64_TYPE
                //    COSEM_DATE_TIME_TYPE
                //    COSEM_DATE_TYPE
                //    COSEM_TIME_TYPE
                //    COSEM_BEGIN_ARRAY
                //        COSEM_ANY_ELEMENT_TYPE
                //    COSEM_END_ARRAY
                //    COSEM_BEGIN_STRUCTURE
                //        COSEM_ANY_ELEMENT_TYPE
                //    COSEM_END_STRUCTURE
                //COSEM_END_CHOICE
            COSEM_END_STRUCTURE
        COSEM_END_ARRAY
    COSEM_END_SCHEMA

    COSEM_BEGIN_SCHEMA(IProfileGeneric_1::Capture_Objects_Schema)
        COSEM_BEGIN_ARRAY
            CAPTURE_OBJECT_DEFINITION
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
        CAPTURE_OBJECT_DEFINITION
    COSEM_END_SCHEMA

    COSEM_BEGIN_SCHEMA(IProfileGeneric_1::Range_Descriptor_Schema)
        COSEM_BEGIN_STRUCTURE
            CAPTURE_OBJECT_DEFINITION   // Restricting Object
            COSEM_BEGIN_CHOICE
                RANGE_VALUE_CHOICES     // Start value
            COSEM_END_CHOICE
            COSEM_BEGIN_CHOICE
                RANGE_VALUE_CHOICES     // End value
            COSEM_END_CHOICE
            COSEM_BEGIN_ARRAY           // List of Capture Objects
                CAPTURE_OBJECT_DEFINITION   // Column (Optional)
            COSEM_END_ARRAY
        COSEM_END_STRUCTURE
    COSEM_END_SCHEMA

    COSEM_BEGIN_SCHEMA(IProfileGeneric_1::Entry_Descriptor_Schema)
        COSEM_BEGIN_STRUCTURE
            COSEM_DOUBLE_LONG_UNSIGNED_TYPE    // From Entry
            COSEM_DOUBLE_LONG_UNSIGNED_TYPE    // To Entry
            COSEM_LONG_UNSIGNED_TYPE           // From Column
            COSEM_LONG_UNSIGNED_TYPE           // To Column
        COSEM_END_STRUCTURE
    COSEM_END_SCHEMA

    //
    // IProfileGeneric_1
    //
    IProfileGeneric_1::IProfileGeneric_1()
    : ICOSEMInterface(CLSID_IProfileGeneric, 0)
    , m_SelectiveAccessRange(Range_Descriptor_Schema)
    , m_SelectiveAccessEntry(Entry_Descriptor_Schema)
    , m_CapturePeriod(0)
    , m_SortMethod(SORT_METHOD_FIFO)
    , m_EntriesInUse(0)
    , m_ProfileEntries(0)
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

    //
    // IProfileGeneric_1
    //
    void IProfileGeneric_1::SetCaptureObjects(const CaptureObjectList& CaptureObjects)
    {
        m_CaptureObjects = CaptureObjects;
    }

    void IProfileGeneric_1::SetCaptureObjects(CaptureObjectList&& CaptureObjects)
    {
        m_CaptureObjects = std::move(CaptureObjects);
    }

    void IProfileGeneric_1::AddCaptureObject(const Cosem_Attribute_Descriptor& Object, uint16_t DataIndex)
    {
        m_CaptureObjects.push_back({ Object, DataIndex });
    }

    void IProfileGeneric_1::RemoveCaptureObject(const Cosem_Attribute_Descriptor& Object)
    {
        for (CaptureObjectList::iterator it = m_CaptureObjects.begin(); it != m_CaptureObjects.end(); ++it)
        {
            if (it->object.class_id == Object.class_id &&
                it->object.instance_id == Object.instance_id &&
                it->object.attribute_id == Object.attribute_id)
            {
                if (m_pSortObject == it)
                    m_pSortObject = m_CaptureObjects.begin();
                m_CaptureObjects.erase(it);
                break;
            }
        }
    }

    void IProfileGeneric_1::ClearCaptureObjects()
    {
        m_CaptureObjects.clear();
    }

    //void IProfileGeneric_1::CaptureData()
    //{
    //    // Default implementation does nothing
    //}

    void IProfileGeneric_1::ResetData()
    {
        m_Buffer.clear();
    }

    void IProfileGeneric_1::SetSortMethod(SortMethodType Method)
    {
        m_SortMethod = Method;
    }

    void IProfileGeneric_1::SetSortObject(const Cosem_Attribute_Descriptor& Object)
    {
        for (CaptureObjectList::iterator it = m_CaptureObjects.begin(); it != m_CaptureObjects.end(); ++it)
        {
            if (it->object.class_id == Object.class_id &&
                it->object.instance_id == Object.instance_id &&
                it->object.attribute_id == Object.attribute_id)
            {
                m_pSortObject = it;
                break;
            }
        }
    }

    void IProfileGeneric_1::SetCapturePeriod(uint32_t Period)
    {
        m_CapturePeriod = Period;
    }

    void IProfileGeneric_1::SetProfileEntries(uint32_t Entries)
    {
        m_ProfileEntries = Entries;
    }

    void IProfileGeneric_1::SetEntriesInUse(uint32_t Entries)
    {
        m_EntriesInUse = Entries;
    }

    void IProfileGeneric_1::SetBuffer(const BufferType& Buffer)
    {
        m_Buffer = Buffer;
    }

    void IProfileGeneric_1::SetBuffer(BufferType&& Buffer)
    {
        m_Buffer = std::move(Buffer);
    }
}
