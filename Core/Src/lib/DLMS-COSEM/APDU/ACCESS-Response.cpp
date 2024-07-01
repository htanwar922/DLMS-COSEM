// Himanshu - ACCESS

#include "APDU/ACCESS-Response.h"

namespace EPRI
{
    ASN_BEGIN_SCHEMA(Access_Response::Access_Response_Schema)
        ASN_BASE_TYPE(ASN::DT_Unsigned32)
        ASN_OCTET_STRING_TYPE(ASN::CONSTRUCTED | ASN::OPTIONAL)
        ASN_DATA_TYPE_WITH_OPTIONS(ASN::NO_OPTIONS)
    ASN_END_SCHEMA

    ASN_BEGIN_SCHEMA(Access_Response::Access_Response_Specification_Schema)
        ASN_BEGIN_CHOICE
            ASN_BEGIN_CHOICE_ENTRY_WITH_OPTIONS(Access_Response::Access_Response_Choice::access_response_get, ASN::IMPLICIT)
                ASN_ENUM_TYPE       // Data Access Result
            ASN_END_CHOICE_ENTRY
        ASN_END_CHOICE
    ASN_END_SCHEMA

    ASN_BEGIN_SCHEMA(Access_Response::Access_Response_Data_Schema)
        ASN_OCTET_STRING_TYPE(ASN::IMPLICIT)
    ASN_END_SCHEMA

    Access_Response::Access_Response()
        : Access_Response_Base::APDUSingleType(Access_Response::Access_Response_Schema)
    {

    }

    Access_Response::~Access_Response()
    {
    }

    bool Access_Response::IsValid() const
    {
        return true;
    }

    bool Access_Response::Parse(DLMSVector * pData,
        COSEMAddressType SourceAddress,
        COSEMAddressType DestinationAddress)
    {
        // Perform the base parse, which just loads
        // the stream.
        //
        if (Access_Response_Base::Parse(pData, SourceAddress, DestinationAddress))
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

                ASNType req_spec_type(Access_Request::Access_Request_Specification_Schema);
                req_spec_type.Parse(&Data);
                while (Length--) {
                    req_spec_type.Rewind(true);
                    AccessRequestSpecification spec;
                    if (ASNType::GetNextResult::VALUE_RETRIEVED != req_spec_type.GetNextValue(&Value)
                            or not req_spec_type.GetChoice((int8_t *)&spec.m_Choice))
                        return false;
                    Position += sizeof(Access_Request::Access_Request_Choice);
                    if (spec.m_Choice == Access_Request::access_request_get) {
                        spec.m_Descriptor.class_id = Value.get<uint16_t>();
                        Position += sizeof(uint16_t);
                        Value.clear();

                        if (ASNType::GetNextResult::VALUE_RETRIEVED != req_spec_type.GetNextValue(&Value))
                            return false;
                        spec.m_Descriptor.instance_id = Value.get<DLMSVector>();
                        Position += Value.get<DLMSVector>().Size();
                        Value.clear();

                        if (ASNType::GetNextResult::VALUE_RETRIEVED != req_spec_type.GetNextValue(&Value))
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

                // AccessResponseListOfData - IEC 62056-5-3:2023 Page 160-162
                Data.SetReadPosition(Position);
                ASNType::GetLength(&Data, &Length);
                Data.RemoveReadBytes();
                Position = 0;

                ASNType data_type(Access_Response_Data_Schema);
                data_type.Parse(&Data);
                COSEMTypeGetVisitor Visitor(data_type.GetBytes());
                while (Length--) {
                    DLMSVector data = Visitor.GetNextValueBytes();
                    if (data.Size() == 0)
                        return false;
                    access_response_list_of_data.push_back(data);
                }
                Data.SetReadPosition(Visitor.GetReadPosition());

                // AccessResponseSpecification - IEC 62056-5-3:2023 Page 160-162
                ASNType::GetLength(&Data, &Length);
                if (Length != access_response_list_of_data.size())
                    return false;

                ASNType spec_type(Access_Response_Specification_Schema);
                spec_type.Parse(&Data);
                while (Length--) {
                    spec_type.Rewind(true);
                    AccessResponseSpecification spec;
                    if (ASNType::GetNextResult::VALUE_RETRIEVED != spec_type.GetNextValue(&Value)
                            or not spec_type.GetChoice((int8_t *)&spec.m_Choice))
                        return false;
                    Position += sizeof(Access_Response::Access_Response_Choice);
                    if (spec.m_Choice == Access_Response::access_response_get) {
                        spec.m_Result = (APDUConstants::Data_Access_Result)Value.get<uint8_t>();
                        Position += sizeof(APDUConstants::Data_Access_Result);
                        Value.clear();

                        access_response_list_of_spec.push_back(spec);
                    }
                    else {
                        throw std::runtime_error("Access Response Specification not implemented");
                    }
                }

                return true;
            }
            catch (const std::exception&)
            {
            }
        }
        return false;
    }

    std::vector<uint8_t> Access_Response::GetBytes()
    {
        m_Type.Clear();
        if (m_Type.SelectChoice(access_response_get))
        {
            m_Type.Append(long_invoke_id_and_priority);
            m_Type.Append(date_time);
            DLMSVector resp;

            ASNType req_spec_type(Access_Request::Access_Request_Specification_Schema);
            for (auto& spec : access_request_list_of_spec)
            {
                req_spec_type.Rewind(true);
                req_spec_type.SelectChoice(spec.m_Choice);
                req_spec_type.Append(spec.m_Descriptor.class_id);
                req_spec_type.Append(spec.m_Descriptor.instance_id);
                req_spec_type.Append(spec.m_Descriptor.attribute_id);
            }
            ASNType::AppendLength(access_request_list_of_spec.size(), &resp);
            resp.Append(req_spec_type.GetBytes());

            ASNType data_type(Access_Response_Data_Schema);
            for (auto& data : access_response_list_of_data)
            {
                data_type.Rewind();
                data_type.Append(data);
            }
            ASNType::AppendLength(access_response_list_of_data.size(), &resp);
            resp.Append(data_type.GetBytes());

            ASNType spec_type(Access_Response_Specification_Schema);
            for (auto& spec : access_response_list_of_spec)
            {
                spec_type.Rewind();
                spec_type.SelectChoice(spec.m_Choice);
                spec_type.Append((uint8_t)spec.m_Result);
            }
            ASNType::AppendLength(access_response_list_of_spec.size(), &resp);
            resp.Append(spec_type.GetBytes());

            m_Type.Append(resp);
            return Access_Response_Base::GetBytes();
        }
        return std::vector<uint8_t>();
    }

}