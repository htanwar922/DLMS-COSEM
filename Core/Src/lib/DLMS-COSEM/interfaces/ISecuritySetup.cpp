// Himanshu

#include "interfaces/ISecuritySetup.h"

namespace EPRI
{
    COSEM_BEGIN_SCHEMA(ISecuritySetup_1::Security_Policy_Schema)
        COSEM_BEGIN_STRUCTURE
            COSEM_UNSIGNED_TYPE
            COSEM_UNSIGNED_TYPE
            COSEM_UNSIGNED_TYPE
            COSEM_UNSIGNED_TYPE
        COSEM_END_STRUCTURE
    COSEM_END_SCHEMA
    COSEM_BEGIN_SCHEMA(ISecuritySetup_1::Security_Suite_Schema)
        COSEM_BEGIN_STRUCTURE
            COSEM_UNSIGNED_TYPE
            COSEM_UNSIGNED_TYPE
            COSEM_UNSIGNED_TYPE
            COSEM_UNSIGNED_TYPE
        COSEM_END_STRUCTURE
    COSEM_END_SCHEMA
    COSEM_BEGIN_SCHEMA(ISecuritySetup_1::Certificates_Schema)
        COSEM_BEGIN_ARRAY
            COSEM_OCTET_STRING_TYPE
        COSEM_END_ARRAY
    COSEM_END_SCHEMA

    //
    // ISecuritySetup_1
    //
    ISecuritySetup_1::ISecuritySetup_1()
        : ICOSEMInterface(CLSID_ISecuritySetup, 1, 0, 1)
    {
        COSEM_ATTRIBUTE(security_policy);
        COSEM_ATTRIBUTE(security_suite);
        COSEM_ATTRIBUTE(client_system_title);
        COSEM_ATTRIBUTE(server_system_title);
        COSEM_ATTRIBUTE(certificates);
        COSEM_METHOD(security_activate);
        COSEM_METHOD(key_transfer);
        COSEM_METHOD(key_agreement);
        COSEM_METHOD(generate_key_pair);
        COSEM_METHOD(generate_certificate_request);
        COSEM_METHOD(import_certificate);
        COSEM_METHOD(export_certificate);
        COSEM_METHOD(remove_certificate);
    }

    ISecuritySetup_1::~ISecuritySetup_1()
    {
    }

    //
    // ISecuritySetupObject
    //
    ISecuritySetupObject::ISecuritySetupObject(const COSEMObjectInstanceCriteria& OIDCriteria,
        uint16_t ShortNameBase /* = std::numeric_limits<uint16_t>::max() */)
        : ICOSEMObject(OIDCriteria, ShortNameBase)
    {
    }

    ISecuritySetupObject::~ISecuritySetupObject()
    {
    }

}