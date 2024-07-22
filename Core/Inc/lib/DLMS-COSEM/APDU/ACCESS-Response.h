// Himanshu - ACCESS

#pragma once

#include "APDU.h"
#include "APDUConstants.h"
#include "COSEM/COSEMTypes.h"
#include "DateTime.h"
#include "interfaces/COSEMData.h"
#include "APDU/ACCESS-Request.h"

namespace EPRI
{
    using Access_Response_Base = APDUSingleType<218>;

    class Access_Response : public Access_Response_Base
    {
        ASN_DEFINE_SCHEMA(Access_Response_Schema)

    public :
        ASN_DEFINE_SCHEMA(Access_Response_Specification_Schema)
        ASN_DEFINE_SCHEMA(Access_Response_Data_Schema)

        Access_Response();
        virtual ~Access_Response();

        enum Access_Response_Choice : int8_t
        {
            access_response_get     = 1,
            access_response_set     = 2,
            access_response_action  = 3,
        };

        struct AccessResponseSpecification
        {
            Access_Response_Choice            m_Choice;
            APDUConstants::Data_Access_Result m_Result;
        };
        typedef DLMSVector AccessResponseData;
        typedef std::vector<AccessResponseSpecification> AccessResponseListOfSpec;
        typedef std::vector<AccessResponseData> AccessResponseListOfData;
        //
        // Attributes
        //
        LongInvokeIdAndPriorityType long_invoke_id_and_priority;
        COSEMDateTime               date_time;
        AccessResponseListOfSpec    access_response_list_of_spec;
        AccessResponseListOfData    access_response_list_of_data;
        AccessRequestListOfSpec     access_request_list_of_spec;

        virtual bool IsValid() const;

        virtual bool Parse(DLMSVector * pData,
            COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress);
        virtual std::vector<uint8_t> GetBytes();
    };

    using AccessResponseSpecification = Access_Response::AccessResponseSpecification;
    using AccessResponseData = Access_Response::AccessResponseData;
    using AccessResponseListOfSpecs = Access_Response::AccessResponseListOfSpec;
    using AccessResponseListOfData = Access_Response::AccessResponseListOfData;


    namespace GLO {
        class Access_Response : public EPRI::GLO::CipheredBase<0x00>
        {
        public:
            Access_Response() = default;
            Access_Response(const Access_Response& Response)
                : EPRI::GLO::CipheredBase<0x00>(Response)
            {
            }
            virtual ~Access_Response() = default;
        };
    }
}
