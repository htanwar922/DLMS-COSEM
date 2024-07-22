// Himanshu

#pragma once

#include "interfaces/ICOSEMInterface.h"

namespace EPRI
{
    const uint16_t CLSID_ISecuritySetup = 64;

    class ISecuritySetup_1 : public ICOSEMInterface
    {
        COSEM_DEFINE_SCHEMA(Security_Policy_Schema)
        COSEM_DEFINE_SCHEMA(Security_Suite_Schema)
        COSEM_DEFINE_SCHEMA(Certificates_Schema)

    public:
        ISecuritySetup_1();
        virtual ~ISecuritySetup_1();

        enum Attributes : ObjectAttributeIdType
        {
            ATTR_SECURITY_POLICY = 2,
            ATTR_SECURITY_SUITE = 3,
            ATTR_CLIENT_SYSTEM_TITLE = 4,
            ATTR_SERVER_SYSTEM_TITLE = 5,
            ATTR_CERTIFICATES = 6
        };

        COSEMAttribute<ATTR_SECURITY_POLICY, Security_Policy_Schema, 0x10>  security_policy;
        COSEMAttribute<ATTR_SECURITY_SUITE, Security_Suite_Schema, 0x18>    security_suite;
        COSEMAttribute<ATTR_CLIENT_SYSTEM_TITLE, OctetStringSchema, 0x20>   client_system_title;
        COSEMAttribute<ATTR_SERVER_SYSTEM_TITLE, OctetStringSchema, 0x28>   server_system_title;
        COSEMAttribute<ATTR_CERTIFICATES, Certificates_Schema, 0x30>        certificates;

        enum Methods : uint8_t
        {
            METHOD_SECURITY_ACTIVATE = 0x30,
            METHOD_KEY_TRANSFER = 0x38,
            METHOD_KEY_AGREEMENT = 0x48,
            METHOD_GENERATE_KEY_PAIR = 0x50,
            METHOD_GENERATE_CERTIFICATE_REQUEST = 0x58,
            METHOD_IMPORT_CERTIFICATE = 0x60,
            METHOD_EXPORT_CERTIFICATE = 0x68,
            METHOD_REMOVE_CERTIFICATE = 0x70
        };

        COSEMMethod<METHOD_SECURITY_ACTIVATE, OctetStringSchema, 0x30>            security_activate;
        COSEMMethod<METHOD_KEY_TRANSFER, OctetStringSchema, 0x38>                 key_transfer;
        COSEMMethod<METHOD_KEY_AGREEMENT, OctetStringSchema, 0x48>                key_agreement;
        COSEMMethod<METHOD_GENERATE_KEY_PAIR, OctetStringSchema, 0x50>            generate_key_pair;
        COSEMMethod<METHOD_GENERATE_CERTIFICATE_REQUEST, OctetStringSchema, 0x58> generate_certificate_request;
        COSEMMethod<METHOD_IMPORT_CERTIFICATE, OctetStringSchema, 0x60>           import_certificate;
        COSEMMethod<METHOD_EXPORT_CERTIFICATE, OctetStringSchema, 0x68>           export_certificate;
        COSEMMethod<METHOD_REMOVE_CERTIFICATE, OctetStringSchema, 0x70>           remove_certificate;
    };

    typedef ISecuritySetup_1 ISecuritySetup;

    class ISecuritySetupObject : public ISecuritySetup, public ICOSEMObject
    {
    public:
        ISecuritySetupObject() = delete;
        ISecuritySetupObject(const COSEMObjectInstanceCriteria& OIDCriteria,
                             uint16_t ShortNameBase = std::numeric_limits<uint16_t>::max());
        virtual ~ISecuritySetupObject();
    };
} // namespace EPRI