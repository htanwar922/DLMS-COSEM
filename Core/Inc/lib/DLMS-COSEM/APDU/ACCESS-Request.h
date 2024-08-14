// Himanshu - ACCESS

#pragma once

#include "APDU.h"
#include "APDUConstants.h"
#include "COSEM/COSEMTypes.h"
#include "DateTime.h"
#include "interfaces/COSEMData.h"

namespace EPRI
{
    using Access_Request_Base = APDUSingleType<217>;

    class Access_Request : public Access_Request_Base
    {
        ASN_DEFINE_SCHEMA(Access_Request_Schema)

    public :
        ASN_DEFINE_SCHEMA(Access_Request_Specification_Schema)
        ASN_DEFINE_SCHEMA(Access_Request_Data_Schema)

        Access_Request();
        virtual ~Access_Request();

        enum Access_Request_Choice : int8_t
        {
            access_request_get                = 1,
            access_request_set                = 2,
            access_request_action             = 3,
            access_request_get_with_selection = 4,
            access_request_set_with_selection = 5
        };

        struct AccessRequestSpecification
        {
            Access_Request_Choice       m_Choice;
            Cosem_Attribute_Descriptor  m_Descriptor;
        };
        typedef DLMSVector AccessRequestData;
        typedef std::vector<AccessRequestSpecification> AccessRequestListOfSpec;
        typedef std::vector<AccessRequestData> AccessRequestListOfData;

        //
        // Attributes
        //
        LongInvokeIdAndPriorityType long_invoke_id_and_priority;
        COSEMDateTime               date_time;
        AccessRequestListOfSpec     access_request_list_of_spec;
        AccessRequestListOfData     access_request_list_of_data;

        virtual bool IsValid() const;

        virtual bool Parse(DLMSVector * pData,
            COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress);
        virtual std::vector<uint8_t> GetBytes();
    };

    using AccessRequestSpecification = Access_Request::AccessRequestSpecification;
    using AccessRequestData = Access_Request::AccessRequestData;
    using AccessRequestListOfSpec = Access_Request::AccessRequestListOfSpec;
    using AccessRequestListOfData = Access_Request::AccessRequestListOfData;


    namespace GLO {
        class Access_Request : public EPRI::GLO::CipheredBase<GLO_ACCESS_REQUEST_TAG>
        {
        public:
            Access_Request() = default;
            Access_Request(const Access_Request& Request)
                : Access_Request::CipheredBase(Request)
            {
            }
            virtual ~Access_Request() = default;
        };
    }
}
