#pragma once

#include "interfaces/ICOSEMInterface.h"

namespace EPRI
{
    const ClassIDType CLSID_IProfileGeneric = 7;

    class IProfileGeneric_1 : public ICOSEMInterface
    {
        COSEM_DEFINE_SCHEMA(Buffer_Schema)
		COSEM_DEFINE_SCHEMA(Capture_Objects_Schema)
		COSEM_DEFINE_SCHEMA(Sort_Method_Schema)
		COSEM_DEFINE_SCHEMA(Sort_Object_Schema)

		COSEM_DEFINE_SCHEMA(Range_Descriptor_Schema)
		COSEM_DEFINE_SCHEMA(Entry_Descriptor_Schema)

    public:

        IProfileGeneric_1();
        virtual ~IProfileGeneric_1();

		enum SortMethodType : uint8_t
		{
			SORT_METHOD_FIFO = 1,
			SORT_METHOD_LIFO = 2,
			SORT_METHOD_LARGEST = 3,
			SORT_METHOD_SMALLEST = 4,
			SORT_METHOD_NEAREST_TO_ZERO = 5,
			SORT_METHOD_FAREST_FROM_ZERO = 6
		};

		struct CaptureObjectType
		{
			Cosem_Attribute_Descriptor object;
			uint16_t data_index;
			operator DLMSStructure() const {
				return DLMSStructure{
					object.class_id,
					object.instance_id,
					object.attribute_id,
					data_index
				};
			}
		};

		enum Attributes : ObjectAttributeIdType
		{
			ATTR_BUFFER = 2,
			ATTR_CAPTURE_OBJECTS = 3,
			ATTR_CAPTURE_PERIOD = 4,
			ATTR_SORT_METHOD = 5,
			ATTR_SORT_OBJECT = 6,
			ATTR_ENTRIES_IN_USE = 7,
			ATTR_PROFILE_ENTRIES = 8
		};

		COSEMAttribute<ATTR_BUFFER, Buffer_Schema, 0x08> buffer;
		COSEMAttribute<ATTR_CAPTURE_OBJECTS, Capture_Objects_Schema, 0x10> capture_objects;
		COSEMAttribute<ATTR_CAPTURE_PERIOD, DoubleLongUnsignedSchema, 0x18> capture_period;
		COSEMAttribute<ATTR_SORT_METHOD, Sort_Method_Schema, 0x20> sort_method;
		COSEMAttribute<ATTR_SORT_OBJECT, Sort_Object_Schema, 0x28> sort_object;
		COSEMAttribute<ATTR_ENTRIES_IN_USE, DoubleLongUnsignedSchema, 0x30> entries_in_use;
		COSEMAttribute<ATTR_PROFILE_ENTRIES, DoubleLongUnsignedSchema, 0x38> profile_entries;

        enum Methods : ObjectAttributeIdType
        {
            METHOD_RESET = 1,
			METHOD_CAPTURE = 2
        };

        COSEMMethod<METHOD_RESET, IntegerSchema, 0x58> reset_data;
		COSEMMethod<METHOD_CAPTURE, IntegerSchema, 0x60> capture_data;

		typedef std::vector<DLMSStructure> BufferType;
		typedef std::vector<CaptureObjectType> CaptureObjectList;
		typedef std::vector<CaptureObjectType>::iterator SortObjectIterator;

		virtual void SetCaptureObjects(const CaptureObjectList& CaptureObjects);
		virtual void SetCaptureObjects(CaptureObjectList&& CaptureObjects);
		virtual void AddCaptureObject(const Cosem_Attribute_Descriptor& Object, uint16_t DataIndex);
		virtual void RemoveCaptureObject(const Cosem_Attribute_Descriptor& Object);
		virtual void ClearCaptureObjects();

		virtual void SetSortMethod(SortMethodType Method);
		virtual void SetSortObject(const Cosem_Attribute_Descriptor& Object);
		virtual void SetCapturePeriod(uint32_t Period);
		virtual void SetProfileEntries(uint32_t Entries);
		virtual void SetEntriesInUse(uint32_t Entries);
		virtual void SetBuffer(const BufferType& Buffer);
		virtual void SetBuffer(BufferType&& Buffer);

		virtual void CaptureData() = 0;
		virtual void ResetData();

	protected:
		SelectiveAccess		m_SelectiveAccessRange;
		SelectiveAccess		m_SelectiveAccessEntry;

		BufferType			m_Buffer;
        CaptureObjectList	m_CaptureObjects;
		uint32_t			m_CapturePeriod;
		SortMethodType		m_SortMethod;
		SortObjectIterator	m_pSortObject;
		uint32_t			m_EntriesInUse;
		uint32_t			m_ProfileEntries;
    };

    typedef IProfileGeneric_1 IProfileGeneric;

    class IProfileGenericObject : public IProfileGeneric, public ICOSEMObject
    {
    public:
    	IProfileGenericObject() = delete;
        IProfileGenericObject(const COSEMObjectInstanceCriteria& OIDCriteria,
            uint16_t ShortNameBase = std::numeric_limits<uint16_t>::max());
        virtual ~IProfileGenericObject();

    };

}
