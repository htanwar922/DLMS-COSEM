// Himanshu

#pragma once

#include "DLMSVector.h"

namespace EPRI
{
    const struct blank {} DLMSBlank;
    template <typename T> using DLMSOptional = std::experimental::optional<T>;
    constexpr auto DLMSOptionalNone = std::experimental::nullopt;
    using DLMSInitList = std::vector<uint32_t>;
    using DLMSBitSet = std::bitset<64>;

    enum DLMSValueIndex : uint8_t
    {
        VAR_BLANK     = 0x00,
        VAR_BOOL      = 0x01,
        VAR_INT8      = 0x02,
        VAR_UINT8     = 0x03,
        VAR_INT16     = 0x04,
        VAR_UINT16    = 0x05,
        VAR_INT32     = 0x06,
        VAR_UINT32    = 0x07,
        VAR_INT64     = 0x08,
        VAR_UINT64    = 0x09,
        VAR_FLOAT     = 0x0a,
        VAR_DOUBLE    = 0x0b,
        VAR_ITERABLES = 0x10,
        VAR_BITSET    = VAR_ITERABLES | 0x01,
        VAR_STRING    = VAR_ITERABLES | 0x02,
        VAR_VECTOR    = VAR_ITERABLES | 0x03,
        VAR_INIT_LIST = VAR_ITERABLES | 0x04,
        VAR_SEQUENCE  = 0x20,
        VAR_STRUCTURE = VAR_SEQUENCE | VAR_ITERABLES | 0x01,
        VAR_ARRAY     = VAR_SEQUENCE | VAR_ITERABLES | 0x02,
        VAR_INVALID   = 0x40
    };

    class bad_variant_access : public std::runtime_error {
    public:
        bad_variant_access(const std::string& what_arg) : runtime_error(what_arg) {}
        bad_variant_access(const char* what_arg) : runtime_error(what_arg) {}
    };

    class DLMSValue {
        using VDT = std::vector<DLMSValue>;
    public:
        struct DLMSStructure : VDT {
            DLMSStructure() : VDT(0) {}
            DLMSStructure(const std::initializer_list<DLMSValue>& li) : VDT(li) {}
        };
        struct DLMSArray : VDT {
            DLMSArray() : VDT(0) {}
            DLMSArray(const std::initializer_list<DLMSValue>& li) : VDT(li) {}
        };

        DLMSValue() : type(VAR_INVALID) { value.blankValue = DLMSBlank; }
        DLMSValue(blank v) : type(VAR_BLANK) { value.blankValue = v; }
        DLMSValue(bool v) : type(VAR_BOOL) { value.boolValue = v; }
        DLMSValue(int8_t v) : type(VAR_INT8) { value.int8Value = v; }
        DLMSValue(uint8_t v) : type(VAR_UINT8) { value.uint8Value = v; }
        DLMSValue(int16_t v) : type(VAR_INT16) { value.int16Value = v; }
        DLMSValue(uint16_t v) : type(VAR_UINT16) { value.uint16Value = v; }
        DLMSValue(int32_t v) : type(VAR_INT32) { value.int32Value = v; }
        DLMSValue(uint32_t v) : type(VAR_UINT32) { value.uint32Value = v; }
        DLMSValue(int64_t v) : type(VAR_INT64) { value.int64Value = v; }
        DLMSValue(uint64_t v) : type(VAR_UINT64) { value.uint64Value = v; }
        DLMSValue(float v) : type(VAR_FLOAT) { value.floatValue = v; }
        DLMSValue(double v) : type(VAR_DOUBLE) { value.doubleValue = v; }
        DLMSValue(const DLMSBitSet& v) : type(VAR_BITSET) { value.bitsetValue = new DLMSBitSet(v); }
        DLMSValue(const std::string& v) : type(VAR_STRING) { value.stringValue = new std::string(v); }
        DLMSValue(char * v) : type(VAR_STRING) { value.stringValue = new std::string(v); }
        DLMSValue(const char * v) : type(VAR_STRING) { value.stringValue = new std::string(v); }
        DLMSValue(const DLMSVector& v) : type(VAR_VECTOR) { value.vectorValue = new DLMSVector(v); }
        DLMSValue(const uint8_t * v, size_t size) : type(VAR_VECTOR) { value.vectorValue = new DLMSVector(v, size); }
        DLMSValue(const std::vector<uint8_t>& v) : type(VAR_VECTOR) { value.vectorValue = new DLMSVector(v); }
        DLMSValue(const DLMSInitList& v) : type(VAR_INIT_LIST) { value.initListValue = new DLMSInitList(v); }
        DLMSValue(const uint32_t * v, size_t size) : type(VAR_INIT_LIST) {
            value.initListValue = new DLMSInitList(size); std::copy(v, v+size, value.initListValue->begin()); }
        DLMSValue(const DLMSStructure& v) : type(VAR_STRUCTURE) { value.structureValue = new DLMSStructure(v); }
        DLMSValue(const DLMSArray& v) : type(VAR_ARRAY) { value.arrayValue = new DLMSArray(v); }
        DLMSValue(const DLMSValue& v) { *this = v; }
        ~DLMSValue() { clear(); }
        void clear() {
            switch (type) {
                case VAR_BITSET:
                    delete value.bitsetValue;
                    break;
                case VAR_STRING:
                    delete value.stringValue;
                    break;
                case VAR_VECTOR:
                    delete value.vectorValue;
                    break;
                case VAR_INIT_LIST:
                    delete value.initListValue;
                    break;
                case VAR_STRUCTURE:
                    delete value.structureValue;
                    break;
                case VAR_ARRAY:
                    delete value.arrayValue;
                    break;
                default:
                    break;
            }
            type = VAR_INVALID;
        }
        DLMSValue& operator=(const DLMSValue& v) {
            // ASSERT(v.type != VAR_INVALID, "DLMSValue.h");
            // ASSERT(type == VAR_INVALID or type == v.type, "DLMSValue.h");
            if (this != &v) {
                clear();
                type = v.type;
                switch (type) {
                    case VAR_BLANK:
                        value.blankValue = v.value.blankValue;
                        break;
                    case VAR_BOOL:
                        value.boolValue = v.value.boolValue;
                        break;
                    case VAR_INT8:
                        value.int8Value = v.value.int8Value;
                        break;
                    case VAR_UINT8:
                        value.uint8Value = v.value.uint8Value;
                        break;
                    case VAR_INT16:
                        value.int16Value = v.value.int16Value;
                        break;
                    case VAR_UINT16:
                        value.uint16Value = v.value.uint16Value;
                        break;
                    case VAR_INT32:
                        value.int32Value = v.value.int32Value;
                        break;
                    case VAR_UINT32:
                        value.uint32Value = v.value.uint32Value;
                        break;
                    case VAR_INT64:
                        value.int64Value = v.value.int64Value;
                        break;
                    case VAR_UINT64:
                        value.uint64Value = v.value.uint64Value;
                        break;
                    case VAR_FLOAT:
                        value.floatValue = v.value.floatValue;
                        break;
                    case VAR_DOUBLE:
                        value.doubleValue = v.value.doubleValue;
                        break;
                    case VAR_BITSET:
                        value.bitsetValue = new DLMSBitSet(*v.value.bitsetValue);
                        break;
                    case VAR_STRING:
                        value.stringValue = new std::string(*v.value.stringValue);
                        break;
                    case VAR_VECTOR:
                        value.vectorValue = new DLMSVector(*v.value.vectorValue);
                        break;
                    case VAR_INIT_LIST:
                        value.initListValue = new DLMSInitList(*v.value.initListValue);
                        break;
                    case VAR_STRUCTURE:
                        value.structureValue = new DLMSStructure(*v.value.structureValue);
                        break;
                    case VAR_ARRAY:
                        value.arrayValue = new DLMSArray(*v.value.arrayValue);
                        break;
                    default:
                        break;
                }
            }
            return *this;
        }
        constexpr DLMSValueIndex which() const {
            return type;
        }
        template <typename T>
        constexpr bool is() const {
            return type == index_of_type<T>();
        }
        constexpr bool is_valid() const {
            return type != VAR_INVALID;
        }
        constexpr bool is_empty() const {
            return type == VAR_BLANK;
        }
        constexpr bool is_iterable() const {
            return type & VAR_ITERABLES;
        }
        constexpr bool is_sequence() const {
            return type & VAR_SEQUENCE;
        }
        template <typename T>
        T& get_unchecked() {
            return const_cast<T&>(static_cast<const DLMSValue*>(this)->get_unchecked<T>());
        }
        template <typename T>
        const T& get_unchecked() const {
            DLMSValueIndex index_of_type_T = index_of_type<T>();
            if (not is_iterable_type<T>())
                return *reinterpret_cast<const T *>(&value);
            return *reinterpret_cast<const T *>(value.pValue);
        }
        template <typename T>
        T& get() {
            if (not is_valid())
                set<T>(T{ });
            return const_cast<T&>(static_cast<const DLMSValue*>(this)->get<T>());
        }
        template <typename T>
        const T& get() const {
            if (const_cast<DLMSValue*>(this)->type == index_of_type<T>())
                return const_cast<DLMSValue*>(this)->get_unchecked<T>();
            else
                throw bad_variant_access("DLMSValue get failed.\r\n");
        }
        template <typename T>
        T get_or(T default_value) {
            return const_cast<T>(static_cast<const DLMSValue*>(this)->get_or(default_value));
        }
        template <typename T>
        T get_or(T default_value) const {
            if (type == index_of_type<T>())
                return get_unchecked<T>();
            else
                return default_value;
        }
        template <typename T>
        operator T() {
            return get<T>();
        }
        template <typename T>
        operator T() const {
            return get<T>();
        }
        template <typename T>
        void set(const T& v = DLMSBlank) {
            clear();
            *this = DLMSValue(v);
            // type = index_of_type<T>();
            // if (not is_iterable())
            //     *this = DLMSValue(v);
            // else
            //     value.pValue = new T(v);
        }
        bool operator==(const DLMSValue& v) const {
            if (type == VAR_INVALID or type != v.type) {
                return false;
            }
            switch (type) {
                case VAR_BLANK:
                    return true;
                case VAR_BOOL:
                    return value.boolValue == v.value.boolValue;
                case VAR_INT8:
                    return value.int8Value == v.value.int8Value;
                case VAR_UINT8:
                    return value.uint8Value == v.value.uint8Value;
                case VAR_INT16:
                    return value.int16Value == v.value.int16Value;
                case VAR_UINT16:
                    return value.uint16Value == v.value.uint16Value;
                case VAR_INT32:
                    return value.int32Value == v.value.int32Value;
                case VAR_UINT32:
                    return value.uint32Value == v.value.uint32Value;
                case VAR_INT64:
                    return value.int64Value == v.value.int64Value;
                case VAR_UINT64:
                    return value.uint64Value == v.value.uint64Value;
                case VAR_FLOAT:
                    return value.floatValue == v.value.floatValue;
                case VAR_DOUBLE:
                    return value.doubleValue == v.value.doubleValue;
                case VAR_BITSET:
                    return *value.bitsetValue == *v.value.bitsetValue;
                case VAR_STRING:
                    return *value.stringValue == *v.value.stringValue;
                case VAR_VECTOR:
                    return *value.vectorValue == *v.value.vectorValue;
                case VAR_INIT_LIST: {
                    if (value.initListValue->size() != v.value.initListValue->size())
                        return false;
                    for (size_t i = 0; i < value.initListValue->size(); i++) {
                        if ((*value.initListValue)[i] != (*v.value.initListValue)[i])
                            return false;
                    }
                    return true;
                }
                case VAR_STRUCTURE:
                    return *value.structureValue == *v.value.structureValue;
                case VAR_ARRAY:
                    return *value.arrayValue == *v.value.arrayValue;
                default:
                    return false;
            }
        }
        bool operator!=(const DLMSValue& v) const {
            return !(*this == v);
        }
        template<typename T>
        DLMSValue& operator=(T x) {
            *this = DLMSValue(x);
            return *this;
        }
        template <typename F>
        __attribute__((__noinline__)) void apply_visitor(F&& f) const
        {
            switch (type) {
                case VAR_BLANK:
                    f(value.blankValue);
                    break;
                case VAR_BOOL:
                    f(value.boolValue);
                    break;
                case VAR_INT8:
                    f(value.int8Value);
                    break;
                case VAR_UINT8:
                    f(value.uint8Value);
                    break;
                case VAR_INT16:
                    f(value.int16Value);
                    break;
                case VAR_UINT16:
                    f(value.uint16Value);
                    break;
                case VAR_INT32:
                    f(value.int32Value);
                    break;
                case VAR_UINT32:
                    f(value.uint32Value);
                    break;
                case VAR_INT64:
                    f(value.int64Value);
                    break;
                case VAR_UINT64:
                    f(value.uint64Value);
                    break;
                case VAR_FLOAT:
                    f(value.floatValue);
                    break;
                case VAR_DOUBLE:
                    f(value.doubleValue);
                    break;
                case VAR_BITSET:
                    f(*value.bitsetValue);
                    break;
                case VAR_STRING:
                    f(*value.stringValue);
                    break;
                case VAR_VECTOR:
                    f(*value.vectorValue);
                    break;
                case VAR_INIT_LIST:
                    f(*value.initListValue);
                    break;
                case VAR_STRUCTURE:
                    f(*value.structureValue);
                    break;
                case VAR_ARRAY:
                    f(*value.arrayValue);
                    break;
                default:
                    break;
            }
        }
    private:
        template <typename T>
        constexpr DLMSValueIndex index_of_type() const {
            if (std::is_same<T, blank>::value) {
                return VAR_BLANK;
            } else if (std::is_same<T, bool>::value) {
                return VAR_BOOL;
            } else if (std::is_same<T, int8_t>::value) {
                return VAR_INT8;
            } else if (std::is_same<T, uint8_t>::value) {
                return VAR_UINT8;
            } else if (std::is_same<T, int16_t>::value) {
                return VAR_INT16;
            } else if (std::is_same<T, uint16_t>::value) {
                return VAR_UINT16;
            } else if (std::is_same<T, int32_t>::value) {
                return VAR_INT32;
            } else if (std::is_same<T, uint32_t>::value) {
                return VAR_UINT32;
            } else if (std::is_same<T, int64_t>::value) {
                return VAR_INT64;
            } else if (std::is_same<T, uint64_t>::value) {
                return VAR_UINT64;
            } else if (std::is_same<T, float>::value) {
                return VAR_FLOAT;
            } else if (std::is_same<T, double>::value) {
                return VAR_DOUBLE;
            } else if (std::is_same<T, DLMSBitSet>::value) {
                return VAR_BITSET;
            } else if (std::is_same<T, std::string>::value) {
                return VAR_STRING;
            } else if (std::is_same<T, DLMSVector>::value) {
                return VAR_VECTOR;
            } else if (std::is_same<T, std::vector<uint8_t>>::value) {
                return VAR_VECTOR;
            } else if (std::is_same<T, DLMSInitList>::value) {
                return VAR_INIT_LIST;
            } else if (std::is_same<T, DLMSStructure>::value) {
                return VAR_STRUCTURE;
            } else if (std::is_same<T, DLMSArray>::value) {
                return VAR_ARRAY;
            } else {
                return VAR_INVALID;
            }
        }
        template <typename T>
        constexpr bool is_iterable_type() const {
            return index_of_type<T>() & VAR_ITERABLES;
        }
        template <typename T>
        constexpr bool is_sequence_type() const {
            return index_of_type<T>() & VAR_SEQUENCE;
        }

        DLMSValueIndex type = VAR_INVALID;
        union value_t {
            blank blankValue;
            bool boolValue;
            int8_t int8Value;
            uint8_t uint8Value;
            int16_t int16Value;
            uint16_t uint16Value;
            int32_t int32Value;
            uint32_t uint32Value;
            int64_t int64Value;
            uint64_t uint64Value;
            float floatValue;
            double doubleValue;
            std::bitset<64> * bitsetValue;
            std::string * stringValue;
            DLMSVector * vectorValue;
            DLMSInitList * initListValue;
            DLMSStructure * structureValue;
            DLMSArray * arrayValue;
            void * pValue = 0;
        } value;
    };

    using DLMSStructure = DLMSValue::DLMSStructure;
    using DLMSArray = DLMSValue::DLMSArray;

    bool IsValueInVariant(const DLMSValue& Value, const DLMSValue& Variant);

    template <typename T, uint8_t BitsToGet /*= 0*/>
	T DLMSVector::Get(bool BigEndian /*= true*/)
	{
		DLMSValue Value;
		if (Get<T, BitsToGet>(&Value, BigEndian))
		{
			return Value.get<T>();
		}
		throw std::out_of_range("Get failed.");
	}
    template <typename _VariantType, uint8_t BitsToGet /*= 0*/>
	bool DLMSVector::Get(DLMSValue * pValue, bool BigEndian /*= true*/)
	{
		size_t BytesPeeked = 0;
		bool   RetVal = Peek<_VariantType, BitsToGet>(pValue, BigEndian, 0, &BytesPeeked);
		if (RetVal)
		{
			m_ReadPosition += BytesPeeked;
		}
		return RetVal;
	}
    template <typename _VariantType, uint8_t BitsToPeek /*= 0*/>
	bool DLMSVector::Peek(DLMSValue * pValue, bool BigEndian /*= true*/,
			size_t Offset /*= 0*/, size_t * pBytesPeeked /*= nullptr*/) const
	{
		static_assert(BitsToPeek == 0 || BitsToPeek == 8 || BitsToPeek == 16 ||
					BitsToPeek == 32 || BitsToPeek == 64,
			"Bits must be 0, 8, 16, 32, or 64");
		static_assert(std::is_integral<_VariantType>::value ||
			std::is_same<_VariantType, float>::value ||
			std::is_same<_VariantType, double>::value,
			"_VariantType must be an integral type, float or double");

		typedef typename
			std::conditional
			<
				BitsToPeek == 0,
				typename std::conditional
				<
					std::is_same<_VariantType, float>::value,
					uint32_t,
					typename std::conditional
					<
						std::is_same<_VariantType, double>::value,
						uint64_t,
						_VariantType
					>::type
				>::type,
				typename std::conditional
				<
					BitsToPeek == 8,
					uint8_t,
					typename std::conditional
					<
						BitsToPeek == 16,
						uint16_t,
						typename std::conditional
						<
							BitsToPeek == 32,
							uint32_t,
							typename std::conditional
							<
								BitsToPeek == 64,
								uint64_t,
								void
							>::type
						>::type
					>::type
				>::type
			>::type PeekBaseType;

		static_assert(sizeof(_VariantType) >= sizeof(PeekBaseType),
			"Variant type is too small");

		if (m_ReadPosition + sizeof(PeekBaseType) + Offset <= m_Data.size())
		{
			PeekBaseType V = 0;
			for (size_t Index = 0; Index < sizeof(PeekBaseType); ++Index)
			{
				if (BigEndian)
				{
					V |= (PeekBaseType(m_Data[m_ReadPosition + Offset + Index]) << ((sizeof(PeekBaseType) - Index - 1) * 8));
				}
				else
				{
					V |= (PeekBaseType(m_Data[m_ReadPosition + Offset +
							(sizeof(PeekBaseType) - Index) - 1]) << ((sizeof(PeekBaseType) - Index - 1) * 8));
				}
			}
			pValue->set<_VariantType>(*((_VariantType *)&V));
			if (nullptr != pBytesPeeked)
			{
				*pBytesPeeked = sizeof(PeekBaseType);
			}
			return true;
		}
		return false;
	}
    template <typename T, uint8_t BitsToGet /*= 0*/>
	T DLMSVector::Peek(size_t Offset /*= 0*/, bool BigEndian /*= true*/) const
	{
		DLMSValue Value;
		if (Peek<T, BitsToGet>(&Value, BigEndian, Offset))
		{
			return Value.get<T>();
		}
		throw std::out_of_range("Peek failed.");
	}

    template <typename T>
	T& DLMSValueGet(DLMSValue& V)
	{
		return V.get<T>();
	}
    template <typename T>
	const T& DLMSValueGet(const DLMSValue& V)
	{
		return V.get<T>();
	}

    inline bool IsValid(const DLMSValue& Value)
    {
        return Value.is_valid();
    }

    inline bool IsVariant(const DLMSValue& Value)
    {
        return IsValid(Value) and not Value.is_sequence();
    }

    inline bool IsSequence(const DLMSValue& Value)
    {
        return Value.is_sequence();
    }

    inline bool IsStructure(const DLMSValue& Value)     // Himanshu - defined and declared
    {
        return Value.which() == VAR_STRUCTURE;
    }

    inline bool IsArray(const DLMSValue& Value)     // Himanshu - defined and declared
    {
        return Value.which() == VAR_ARRAY;
    }

    inline bool IsBlank(const DLMSValue& Value)
    {
        return Value.which() == VAR_BLANK;
    }

    inline bool IsInitialized(const DLMSValue& Value)
    {
        return Value.is_valid() and not Value.is_empty();
    }

    inline DLMSValueIndex VariantType(const DLMSValue& Value)
    {
        return DLMSValueIndex(Value.which());
    }

    template <typename T>
        inline DLMSValue MakeVariant(const DLMSOptional<T>& Optional)
        {
            if (Optional)
            {
                return Optional.value();
            }
            else
            {
                return DLMSBlank;
            }
        }

    inline DLMSStructure& DLMSValueGetStructure(DLMSValue& V)       // Himanshu - defined and declared
    {
        return V.get<DLMSStructure>();
    }

    inline DLMSArray& DLMSValueGetArray(DLMSValue& V)       // Himanshu - defined and declared
    {
        return V.get<DLMSArray>();
    }

    inline size_t DLMSValueGetStructureSize(const DLMSValue& V)       // Himanshu - defined and declared
    {
        return V.get<DLMSStructure>().size();
    }

    inline size_t DLMSValueGetArraySize(const DLMSValue& V)       // Himanshu - defined and declared
    {
        return V.get<DLMSArray>().size();
    }

    inline const DLMSStructure& DLMSValueGetStructure(const DLMSValue& V)       // Himanshu - defined and declared
    {
        return V.get<DLMSStructure>();
    }

    inline const DLMSArray& DLMSValueGetArray(const DLMSValue& V)       // Himanshu - defined and declared
    {
        return V.get<DLMSArray>();
    }

    inline DLMSValue& DLMSValueGetVariant(DLMSValue& V)
    {
        return V;
    }

    inline const DLMSValue& DLMSValueGetVariant(const DLMSValue& V)
    {
        return V;
    }

#if LOGGING_AT_OR_BELOW(LOG_LVL_DEBUG)
    void Print(DLMSValue * V, std::string pre = "");
    void Print(DLMSStructure * V, std::string pre = "");
    void Print(DLMSArray * V, std::string pre = "");
#endif

} // namespace EPRI
