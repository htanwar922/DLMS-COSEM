// Himanshu

#include <algorithm>

#include "DLMSVector.h"
#include "DLMSValue.h"

namespace EPRI
{
    bool IsValueInVariant(const DLMSValue& Value, const DLMSValue& Variant)
    {
        bool RetVal = true;
        uint32_t    CompareValue = 0;
        switch (Variant.which())
        {
        case VAR_INIT_LIST:
            {
                switch (Value.which())
                {
                case VAR_INT8:
                    CompareValue = Value.get<int8_t>();
                    break;
                case VAR_UINT8:
                    CompareValue = Value.get<uint8_t>();
                    break;
                case VAR_INT16:
                    CompareValue = Value.get<int16_t>();
                    break;
                case VAR_UINT16:
                    CompareValue = Value.get<uint16_t>();
                    break;
                case VAR_INT32 :
                    CompareValue = Value.get<int32_t>();
                    break;
                case VAR_UINT32:
                    CompareValue = Value.get<uint32_t>();
                    break;
                default:
                    RetVal = false;
                    break;
                }
            }
        default:
            break;
        }
        if (RetVal)
        {
            RetVal = std::find(Variant.get<DLMSInitList>().begin(),
                		Variant.get<DLMSInitList>().end(),
                		CompareValue) != Variant.get<DLMSInitList>().end();
        }
        return RetVal;
    }

    class AppendVisitor //: public static_visitor<>
    {
    public:
        AppendVisitor() = delete;
        AppendVisitor(DLMSVector * pVector, bool Trim = true)
            : m_pVector(pVector)
            , m_Trim(Trim)
        {
        }

        void operator()(const blank& Value) {}
        void operator()(const bool& Value) {
            m_pVector->Append<uint8_t>(Value ? 1 : 0);
        }
        void operator()(const int8_t& Value) {
            m_pVector->Append(Value);
        }
        void operator()(const uint8_t& Value) {
            m_pVector->Append(Value);
        }
        void operator()(const int16_t& Value) {
            if (!m_Trim)
                m_pVector->Append(Value);
            else
                AppendTrimmedInteger(Value);
        }
        void operator()(const uint16_t& Value) {
            if (!m_Trim)
                m_pVector->Append(Value);
            else
                AppendTrimmedInteger(Value);
        }
        void operator()(const int32_t& Value) {
            if (!m_Trim)
                m_pVector->Append(Value);
            else
                AppendTrimmedInteger(Value);
        }
        void operator()(const uint32_t& Value) {
            if (!m_Trim)
                m_pVector->Append(Value);
            else
                AppendTrimmedInteger(Value);
        }
        void operator()(const int64_t& Value) {
            if (!m_Trim)
                m_pVector->Append(Value);
            else
                AppendTrimmedInteger(Value);
        }
        void operator()(const uint64_t& Value) {
            if (!m_Trim)
                m_pVector->Append(Value);
            else
                AppendTrimmedInteger(Value);
        }
        void operator()(const std::string& Value) {
            m_pVector->Append(Value);
        }
        void operator()(const float& Value) {
            m_pVector->AppendFloat(Value);
        }
        void operator()(const double& Value) {
            m_pVector->AppendDouble(Value);
        }
        void operator()(const DLMSVector& Value) {
            m_pVector->Append(Value);
        }
        void operator()(const DLMSInitList& Value)
        {
            throw std::logic_error("Not implemented");
        }
        void operator()(const DLMSBitSet& Value) {
            m_pVector->Append(Value.to_ullong());
        }
        void operator()(const DLMSStructure& Value) {
            throw std::logic_error("Not implemented");
        }
        void operator()(const DLMSArray& Value) {
            throw std::logic_error("Not implemented");
        }

    protected:
        void AppendTrimmedInteger(uint64_t Value)
        {
            DLMSVector Vector;
            size_t        VectorIndex = 0;
            Vector.Append(Value);
            for (; VectorIndex < sizeof(uint64_t); VectorIndex += 2)
            {
                if (Vector[VectorIndex] ||
                    Vector[VectorIndex + 1])
                {
                    break;
                }
            }
            if (6 == VectorIndex && Vector[VectorIndex] == 0x00)
            {
                ++VectorIndex;
            }
            if (VectorIndex == sizeof(uint64_t))
            {
                m_pVector->Append<uint8_t>(0x00);
            }
            else
            {
                m_pVector->Append(Vector, VectorIndex, sizeof(uint64_t) - VectorIndex);
            }
        }

        DLMSVector * m_pVector;
        bool         m_Trim;
    };

    size_t DLMSVector::Append(const DLMSValue& Value, bool Trim /* = true*/)
    {
        size_t RetVal = m_Data.size();
        AppendVisitor Visitor(this, Trim);
        Value.apply_visitor(Visitor);
        return RetVal;
    }


#if LOGGING_AT_OR_BELOW(LOG_LVL_DEBUG)

    void Print(DLMSStructure * V, std::string pre /*= ""*/) {
        printf("%sStructure :\r\n", pre.c_str());
        for(size_t i=0; i<V->size(); i++) {
            Print(&V->at(i), pre + '\t');
            printf("\r\n");
        }
    }
    void Print(DLMSArray * V, std::string pre /*= ""*/) {
        printf("%sArray :\r\n", pre.c_str());
        for(size_t i=0; i<V->size(); i++) {
            Print(&V->at(i), pre + '\t');
            printf("\r\n");
        }
    }
    void Print(DLMSValue * V, std::string pre /*= ""*/) {
        switch (V->which()) {
        case VAR_BLANK:
            printf("%s_/\\_ ", pre.c_str());
            break;
        case VAR_BOOL:
            printf("%s0x%02x ", pre.c_str(), V->get<bool>());
            break;
        case VAR_INT8:
            printf("%s0x%02x ", pre.c_str(), V->get<int8_t>());
            break;
        case VAR_UINT8:
            printf("%s0x%02x ", pre.c_str(), V->get<uint8_t>());
            break;
        case VAR_INT16:
            printf("%s0x%04x ", pre.c_str(), V->get<int16_t>());
            break;
        case VAR_UINT16:
            printf("%s0x%04x ", pre.c_str(), V->get<uint16_t>());
            break;
        case VAR_INT32:
#ifdef STM32
            printf("%s0x%08lx ", pre.c_str(), V->get<int32_t>());
#else
            printf("%s0x%08x ", pre.c_str(), V->get<int32_t>());
#endif
            break;
        case VAR_UINT32:
#ifdef STM32
            printf("%s0x%08lx ", pre.c_str(), V->get<uint32_t>());
#else
            printf("%s0x%08x ", pre.c_str(), V->get<uint32_t>());
#endif
            break;
        case VAR_INT64:
#ifdef STM32
            printf("%s0x%016llx ", pre.c_str(), V->get<int64_t>());
#else
            printf("%s0x%016lx ", pre.c_str(), V->get<int64_t>());
#endif
            break;
        case VAR_UINT64:
#ifdef STM32
            printf("%s0x%016llx ", pre.c_str(), V->get<uint64_t>());
#else
            printf("%s0x%016lx ", pre.c_str(), V->get<uint64_t>());
#endif
            break;
        case VAR_STRING:
            printf("%s%s ", pre.c_str(), V->get<std::string>().c_str());
            break;
        case VAR_FLOAT:
            printf("%s%f ", pre.c_str(), V->get<float>());
            break;
        case VAR_DOUBLE:
            printf("%s%f ", pre.c_str(), V->get<double>());
            break;
        case VAR_VECTOR:
            for(size_t i=0; i<V->get<DLMSVector>().Size(); i++)
                printf("%s0x%02x ", pre.c_str(), V->get<DLMSVector>()[i]);
            printf("\r\n");
            break;
        case VAR_INIT_LIST:
            printf("%sList: ", pre.c_str());
            for(size_t i=0; i<V->get<DLMSInitList>().size(); i++)
#ifdef STM32
                printf("%s0x%08lx ", pre.c_str(), V->get<DLMSInitList>()[i]);
#else
                printf("%s0x%08x ", pre.c_str(), V->get<DLMSInitList>()[i]);
#endif
            printf("\r\n");
            break;
        case VAR_BITSET: {
            std::ostringstream os;
            os << V->get<DLMSBitSet>();
            printf("%s%s ", pre.c_str(), os.str().c_str());
            break;
        }
        case VAR_STRUCTURE:
            Print(&V->get<DLMSStructure>(), pre);
            break;
        case VAR_ARRAY:
            Print(&V->get<DLMSArray>(), pre);
            break;
        default:
            break;
        }
    }

#endif
}
