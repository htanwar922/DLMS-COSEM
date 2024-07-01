// Himanshu - ACCESS

#include "APDU/ACCESS-Request.h"

namespace EPRI
{
    ASN_BEGIN_SCHEMA(Access_Request::Access_Request_Schema)
        ASN_BASE_TYPE(ASN::DT_Unsigned32)
        ASN_OCTET_STRING_TYPE(ASN::OPTIONAL)
        ASN_DATA_TYPE_WITH_OPTIONS(ASN::NO_OPTIONS)
    ASN_END_SCHEMA

    ASN_BEGIN_SCHEMA(Access_Request::Access_Request_Specification_Schema)
        ASN_BEGIN_CHOICE
            ASN_BEGIN_CHOICE_ENTRY_WITH_OPTIONS(Access_Request::Access_Request_Choice::access_request_get, ASN::IMPLICIT)
                ASN_BASE_TYPE(ASN::DT_Unsigned16)
                ASN_FIXED_OCTET_STRING_TYPE(ASN::IMPLICIT, 6)
                ASN_BASE_TYPE(ASN::DT_Unsigned8)
            ASN_END_CHOICE_ENTRY
        ASN_END_CHOICE
    ASN_END_SCHEMA

    ASN_BEGIN_SCHEMA(Access_Request::Access_Request_Data_Schema)
        ASN_DATA_TYPE_WITH_OPTIONS(ASN::OPTIONAL)
    ASN_END_SCHEMA

    Access_Request::Access_Request()
        : Access_Request_Base(Access_Request::Access_Request_Schema)
    {

    }

    Access_Request::~Access_Request()
    {
    }

    bool Access_Request::IsValid() const
    {
        return true;
    }

    bool Access_Request::Parse(DLMSVector * pData,
        COSEMAddressType SourceAddress,
        COSEMAddressType DestinationAddress)
    {
        // Perform the base parse, which just loads
        // the stream.
        //
        if (Access_Request_Base::Parse(pData, SourceAddress, DestinationAddress))
        {
            DLMSValue    Value;

            m_Type.Rewind();
            try
            {
                if (ASNType::GetNextResult::VALUE_RETRIEVED != m_Type.GetNextValue(&Value))
                    return false;
                long_invoke_id_and_priority = Value.get<uint32_t>();
                Value.clear();
                if (ASNType::GetNextResult::VALUE_RETRIEVED != m_Type.GetNextValue(&Value))
                    return false;
                date_time = Value;
                Value.clear();
                if (ASNType::GetNextResult::VALUE_RETRIEVED != m_Type.GetNextValue(&Value))
                    return false;
                DLMSVector Data = Value.get<DLMSVector>();
                Value.clear();

                // AccessRequestSpecification - IEC 62056-5-3:2023 Page 160-162
                size_t Length = 0;
                ASNType::GetLength(&Data, &Length);
                size_t Position = Data.GetReadPosition();

                ASNType spec_type(Access_Request_Specification_Schema);
                spec_type.Parse(&Data);
                while (Length--) {
                    spec_type.Rewind(true);
                    AccessRequestSpecification spec;
                    if (ASNType::GetNextResult::VALUE_RETRIEVED != spec_type.GetNextValue(&Value)
                            or not spec_type.GetChoice((int8_t *)&spec.m_Choice))
                        return false;
                    Position += sizeof(Access_Request_Choice);
                    if (spec.m_Choice == Access_Request_Choice::access_request_get) {
                        spec.m_Descriptor.class_id = Value.get<uint16_t>();
                        Position += sizeof(uint16_t);
                        Value.clear();

                        if (ASNType::GetNextResult::VALUE_RETRIEVED != spec_type.GetNextValue(&Value))
                            return false;
                        spec.m_Descriptor.instance_id = Value.get<DLMSVector>();
                        Position += Value.get<DLMSVector>().Size();
                        Value.clear();

                        if (ASNType::GetNextResult::VALUE_RETRIEVED != spec_type.GetNextValue(&Value))
                            return false;
                        spec.m_Descriptor.attribute_id = Value.get<uint8_t>();
                        Position += sizeof(uint8_t);
                        Value.clear();

                        access_request_list_of_spec.push_back(spec);
                    }
                    else {
                        throw std::runtime_error("Access Request Specification not implemented");
                    }
                }

                // AccessRequestListOfData - IEC 62056-5-3:2023 Page 160-162
                Data.SetReadPosition(Position);
                ASNType::GetLength(&Data, &Length);
                if (Length != access_request_list_of_spec.size())
                    return false;
                Data.RemoveReadBytes();
                Position = 0;

                ASNType data_type(Access_Request_Data_Schema);
                data_type.Parse(&Data);
                COSEMTypeGetVisitor Visitor(data_type.GetBytes());
                while (Length--) {
                    DLMSVector data = Visitor.GetNextValueBytes();
                    if (data.Size() == 0)
                        return false;
                    access_request_list_of_data.push_back(data);
                }
                Data.SetReadPosition(Visitor.GetReadPosition());
                return true;
            }
            catch (const std::exception&)
            {
            }
        }
        return false;
    }

    std::vector<uint8_t> Access_Request::GetBytes()
    {
        m_Type.Clear();
        if (m_Type.SelectChoice(access_request_get))
        {
            m_Type.Append(long_invoke_id_and_priority);
            m_Type.Append(date_time);

            DLMSVector req;

            ASNType spec_type(Access_Request_Specification_Schema);
            for (auto& spec : access_request_list_of_spec)
            {
                spec_type.Rewind(true);
                spec_type.SelectChoice(spec.m_Choice);
                spec_type.Append(spec.m_Descriptor.class_id);
                spec_type.Append(spec.m_Descriptor.instance_id);
                spec_type.Append(spec.m_Descriptor.attribute_id);
            }
            ASNType::AppendLength(access_request_list_of_spec.size(), &req);
            req.Append(spec_type.GetBytes());

            ASNType data_type(Access_Request_Data_Schema);
            for (auto& data : access_request_list_of_data)
            {
                data_type.Rewind();
                data_type.Append(data);
            }
            ASNType::AppendLength(access_request_list_of_data.size(), &req);
            req.Append(data_type.GetBytes());

            m_Type.Append(req);
            return Access_Request_Base::GetBytes();
        }
        return std::vector<uint8_t>();
    }
}