
#include "COSEM/COSEM.h"
#include "Device/Association.h"

namespace EPRI
{
    //
    // AssociationContext
    //
    AssociationContext::AssociationContext()
        : m_ClientSAP(INVALID_ADDRESS)
        , m_ServerSAP(INVALID_ADDRESS)
        , m_SecurityOptions(SecurityContextType::NO_SUITE, SecurityContextType::no_policy)
        , m_xDLMS()
        , m_Status(AssociationStatusType::non_associated)
        , m_Initialized(false)
    {
    }

    //AssociationContext::AssociationContext(const APPOpenConfirmOrResponse& Response)
    //    : m_ClientSAP(Response.m_DestinationAddress)
    //    , m_ServerSAP(Response.m_SourceAddress)
    //    , m_SecurityOptions(Response.m_SecurityOptions)
    //    , m_xDLMS(Response.m_xDLMS)
    //    , m_Status(AssociationStatusType::non_associated)
    //    , m_Initialized(true)
    //{
    //}

    AssociationContext& AssociationContext::operator=(const AssociationContext& RHS)
    {
        m_ClientSAP = RHS.m_ClientSAP;
        m_ServerSAP = RHS.m_ServerSAP;
        m_SecurityOptions = RHS.m_SecurityOptions;
        m_xDLMS = RHS.m_xDLMS;
        m_Status = RHS.m_Status;
        m_Initialized = RHS.m_Initialized;
        return *this;
    }

    bool AssociationContext::Validate(const APPOpenConfirmOrResponse& Response)
    {
        //if (m_ServerSAP != Response.m_SourceAddress)
        //{
        //    return false;
        //}
        if (m_SecurityOptions.MechanismName != Response.m_SecurityOptions.MechanismName)
        {
            return false;
        }
        if (m_SecurityOptions.MechanismName == COSEMSecurityOptions::MechanismNameLowLevelSecurity)
        {
            if (m_SecurityOptions.AuthenticationValue != Response.m_SecurityOptions.CtoS)
            {
                return false;
            }
        }
        else if (m_SecurityOptions.MechanismName == COSEMSecurityOptions::MechanismNameHighLevelSecurity
            or m_SecurityOptions.MechanismName == COSEMSecurityOptions::MechanismNameHighLevelSecurityMD5
            or m_SecurityOptions.MechanismName == COSEMSecurityOptions::MechanismNameHighLevelSecuritySHA1
            or m_SecurityOptions.MechanismName == COSEMSecurityOptions::MechanismNameHighLevelSecurityGMAC
            or m_SecurityOptions.MechanismName == COSEMSecurityOptions::MechanismNameHighLevelSecuritySHA256
            or m_SecurityOptions.MechanismName == COSEMSecurityOptions::MechanismNameHighLevelSecurityECDSA)
        {
            //m_SecurityOptions.AuthenticationValue = Response.m_SecurityOptions.AuthenticationValue;
        }
        if (m_SecurityOptions.ApplicationContextName != Response.m_SecurityOptions.ApplicationContextName)
        {
            return false;
        }
        return true;
    }

    //
    // Association
    /*
     *	0	0	40	0	0	255	Current association     12	15		Class 12 is SN referencing and Class 15 is LN referencing.
     *	0	0	40	0	1	255	PC Association	        12	15		LN without ciphering. Non auth. No enc.
     *	0	0	40	0	2   255	MR Association          12	15		LN-ciphered. LLS auth. Encrypted data txf.
     *	0	0	40	0	3   255	US Association          12	15		LN-ciphered. HLS auth. Encrypted data txf.
     *	0	0	40	0	4   255	PUSH Association        12	15		LN-ciphered. Non auth. Encrypted data txf.
     *	0	0	40	0	5   255	FOTA Association        12	15		LN-ciphered. HLS auth. Encrypted data txf.
     *	0	0	40	0	6   255	IHD Association         12	15		LN-ciphered. LLS auth. Encrypted data txf.
    */
    Association::Association(std::vector<ICOSEMObject*>* pObjects) :
        IAssociationLNObject({ 0, 0, 40, 0, {0, 6}, 255 }),
        m_pObjects(pObjects)
    {
        const int N = 6 + 1;
        m_Contexts.reserve(N);
        m_Contexts.resize(N);
        m_ObjectsByContext.reserve(N);
        m_ObjectsByContext.resize(N);
        for (uint8_t i = 0; i < N; i++)
        {
            RegisterObjectInstanceID({ 0, 0, 40, 0, i, 255 });
            SetAttributeAccessRights({ 0, 0, 40, 0, i, 255 }, LOGICAL_NAME, attr_read_access);
            SetAttributeAccessRights({ 0, 0, 40, 0, i, 255 }, ATTR_OBJ_LIST, attr_read_access);
            SetAttributeAccessRights({ 0, 0, 40, 0, i, 255 }, ATTR_PARTNERS_ID, attr_read_access);
            SetAttributeAccessRights({ 0, 0, 40, 0, i, 255 }, ATTR_APP_CTX_NAME, attr_read_access);
            SetAttributeAccessRights({ 0, 0, 40, 0, i, 255 }, ATTR_XDLMS_CTX_INFO, attr_read_access);
            SetAttributeAccessRights({ 0, 0, 40, 0, i, 255 }, ATTR_AUTH_MECH_NAME, attr_read_access);
            SetAttributeAccessRights({ 0, 0, 40, 0, i, 255 }, ATTR_SECRET, access_denied);
            SetAttributeAccessRights({ 0, 0, 40, 0, i, 255 }, ATTR_STATUS, attr_read_access);
            SetAttributeAccessRights({ 0, 0, 40, 0, i, 255 }, ATTR_SECURITY_SETUP_REF, attr_read_access);

            switch (i)
            {
            case 0: // For Current Association
                break;
            case 1: // PC Association
                m_Contexts[i].m_SecurityOptions.ApplicationContextName = COSEMSecurityOptions::ContextLNRNoCipher;
                //m_Contexts[i].m_SecurityOptions.MechanismName = COSEMSecurityOptions::MechanismNameNoSecurity;
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetPolicyBit(SecurityContextType::no_policy);
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetSecuritySuite(SecuritySuite_None());
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetSecuritySetupObjectID({ 0, 0, 0, 0, 0, 0 });
                m_Contexts[i].m_xDLMS.SetConformance(
                    1 << xDLMS::ConformanceBits::get
                );
                m_ObjectsByContext[i] = AssociationConfigPC;
                break;
            case 2: // MR Association
                m_Contexts[i].m_SecurityOptions.ApplicationContextName = COSEMSecurityOptions::ContextLNRCipher;
                m_Contexts[i].m_SecurityOptions.MechanismName = COSEMSecurityOptions::MechanismNameLowLevelSecurity;
                m_Contexts[i].m_SecurityOptions.AuthenticationValue = ASSOCIATION_MR_AUTHENTICATION_VALUE;
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetPolicyBit(SecurityContextType::authenticated_request);
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetPolicyBit(SecurityContextType::encrypted_request);
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetSecuritySuite(SecuritySuite_0(glo_KEY, glo_AAD));
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetSecuritySetupObjectID({ 0, 0, 43, 0, 2, 255 });
                m_Contexts[i].m_xDLMS.SetConformance(
                    1 << xDLMS::ConformanceBits::get |
                    1 << xDLMS::ConformanceBits::block_transfer_with_get_or_read |
                    1 << xDLMS::ConformanceBits::selective_access
                );
                m_ObjectsByContext[i] = AssociationConfigMR;
                break;
            case 3: // US Association
                m_Contexts[i].m_SecurityOptions.ApplicationContextName = COSEMSecurityOptions::ContextLNRCipher;
                m_Contexts[i].m_SecurityOptions.MechanismName = COSEMSecurityOptions::MechanismNameHighLevelSecurity;
                m_Contexts[i].m_SecurityOptions.AuthenticationValue = DLMSVector(ASSOCIATION_US_AUTHENTICATION_VALUE.c_str(), ASSOCIATION_US_AUTHENTICATION_VALUE.size());
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetPolicyBit(SecurityContextType::authenticated_request);
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetPolicyBit(SecurityContextType::encrypted_request);
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetSecuritySuite(SecuritySuite_0(glo_KEY, glo_AAD));
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetSecuritySetupObjectID({ 0, 0, 43, 0, 3, 255 });
                m_Contexts[i].m_xDLMS.SetConformance(
                    1 << xDLMS::ConformanceBits::get |
                    1 << xDLMS::ConformanceBits::set |
                    1 << xDLMS::ConformanceBits::action |
                    1 << xDLMS::ConformanceBits::selective_access |
                    1 << xDLMS::ConformanceBits::block_transfer_with_get_or_read |
                    1 << xDLMS::ConformanceBits::block_transfer_with_set_or_write |
                    1 << xDLMS::ConformanceBits::selective_access |
                    1 << xDLMS::ConformanceBits::data_notification |
                    1 << xDLMS::ConformanceBits::general_block_transfer
                );
                m_ObjectsByContext[i] = AssociationConfigUS;
                break;
            case 4: // PUSH Association
                m_Contexts[i].m_SecurityOptions.ApplicationContextName = COSEMSecurityOptions::ContextLNRCipher;
                m_Contexts[i].m_SecurityOptions.MechanismName = COSEMSecurityOptions::MechanismNameNoSecurity;
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetPolicyBit(SecurityContextType::authenticated_request);
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetPolicyBit(SecurityContextType::encrypted_request);
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetSecuritySuite(SecuritySuite_0(glo_KEY, glo_AAD));
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetSecuritySetupObjectID({ 0, 0, 43, 0, 4, 255 });
                m_Contexts[i].m_xDLMS.SetConformance(
                    1 << xDLMS::ConformanceBits::data_notification |
                    1 << xDLMS::ConformanceBits::general_block_transfer
                );
                m_ObjectsByContext[i] = AssociationConfigPUSH;
                break;
            case 5: // FOTA Association
                m_Contexts[i].m_SecurityOptions.ApplicationContextName = COSEMSecurityOptions::ContextLNRCipher;
                m_Contexts[i].m_SecurityOptions.MechanismName = COSEMSecurityOptions::MechanismNameHighLevelSecurity;
                m_Contexts[i].m_SecurityOptions.AuthenticationValue = DLMSVector(ASSOCIATION_FOTA_AUTHENTICATION_VALUE.c_str(), ASSOCIATION_FOTA_AUTHENTICATION_VALUE.size());
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetPolicyBit(SecurityContextType::authenticated_request);
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetPolicyBit(SecurityContextType::encrypted_request);
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetSecuritySuite(SecuritySuite_0(glo_KEY, glo_AAD));
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetSecuritySetupObjectID({ 0, 0, 43, 0, 5, 255 });
                m_Contexts[i].m_xDLMS.SetConformance(
                    1 << xDLMS::ConformanceBits::get |
                    1 << xDLMS::ConformanceBits::set |
                    1 << xDLMS::ConformanceBits::action |
                    1 << xDLMS::ConformanceBits::block_transfer_with_set_or_write
                );
                m_ObjectsByContext[i] = AssociationConfigFOTA;
                break;
            case 6: // IHD Association
                m_Contexts[i].m_SecurityOptions.ApplicationContextName = COSEMSecurityOptions::ContextLNRCipher;
                m_Contexts[i].m_SecurityOptions.MechanismName = COSEMSecurityOptions::MechanismNameLowLevelSecurity;
                m_Contexts[i].m_SecurityOptions.AuthenticationValue = ASSOCIATION_IHD_AUTHENTICATION_VALUE;
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetPolicyBit(SecurityContextType::authenticated_request);
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetPolicyBit(SecurityContextType::encrypted_request);
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetSecuritySuite(SecuritySuite_0(glo_KEY, glo_AAD));
                m_Contexts[i].m_SecurityOptions.SecurityContext.SetSecuritySetupObjectID({ 0, 0, 43, 0, 6, 255 });
                m_Contexts[i].m_xDLMS.SetConformance(
                    1 << xDLMS::ConformanceBits::get |
                    1 << xDLMS::ConformanceBits::set |
                    1 << xDLMS::ConformanceBits::block_transfer_with_get_or_read
                );
                m_ObjectsByContext[i] = AssociationConfigIHD;
                break;
            }

        }
    }

    Association::Association(std::vector<ICOSEMObject*>* pObjects
        , const COSEMObjectInstanceID& OID
        , uint16_t ShortNameBase /*= std::numeric_limits<uint16_t>::max()*/)
        : IAssociationLNObject(OID, ShortNameBase)
        , m_pObjects(pObjects)
    {
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_OBJ_LIST, attr_read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_PARTNERS_ID, attr_read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_APP_CTX_NAME, attr_read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_XDLMS_CTX_INFO, attr_read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_AUTH_MECH_NAME, attr_read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_SECRET, access_denied);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_STATUS, attr_read_access);
        SetAttributeAccessRights(m_InstanceCriteria, ATTR_SECURITY_SETUP_REF, attr_read_access);
    }

    Association::~Association()
    {
    }

    size_t Association::AvailableAssociations() const
    {
        //
        // TODO - Only 1 association allowed for this release. Phase II.
        //
        //return 1 - m_Contexts.size();
        //return not std::any_of(m_Contexts.begin(), m_Contexts.end(), [](const AssociationContext& Entry) -> bool { return Entry.m_Initialized; });
        //return std::any_of(m_Contexts.begin(), m_Contexts.end(), [](const AssociationContext& Entry) -> bool { return not Entry.m_Initialized; });
        return m_pCurrentContext == nullptr or not m_pCurrentContext->m_Initialized;
    }

    const std::shared_ptr<ISecuritySuite> Association::GetSecuritySetup(COSEMAddressType Address) const
    {
        if (Address == INVALID_ADDRESS)
        {
            return nullptr;
        }
        //if (GetAssociationContextByAddress(Address))
        //{
        //    return GetAssociationContextByAddress(Address)->m_SecurityOptions.SecurityContext.GetSecuritySuite();
        //}
        uint8_t AssociationIndex = 0;
        switch (Address)
        {
        case 0x10:
            AssociationIndex = 1;
            break;
        case 0x20:
            AssociationIndex = 2;
            break;
        case 0x30:
            AssociationIndex = 3;
            break;
        case 0x40:
            AssociationIndex = 4;
            break;
        case 0x50:
            AssociationIndex = 5;
            break;
        case 0x60:
            AssociationIndex = 6;
            break;
        default:
            return nullptr;
        }
        return m_Contexts[AssociationIndex].m_SecurityOptions.SecurityContext.GetSecuritySuite();
    }

    bool Association::RegisterAssociation(const APPOpenConfirmOrResponse& Response, const xDLMS::Context Context)
    {
        if (AvailableAssociations())
        {
            uint8_t AssociationIndex = 0;
            switch (Response.m_DestinationAddress)
            {
            case 0x10:
                AssociationIndex = 1;
                break;
            case 0x20:
                AssociationIndex = 2;
                break;
            case 0x30:
                AssociationIndex = 3;
                break;
            case 0x40:
                AssociationIndex = 4;
                break;
            case 0x50:
                AssociationIndex = 5;
                break;
            case 0x60:
                AssociationIndex = 6;
                break;
            default:
                return false;
            }
            //m_Contexts[AssociationIndex] = AssociationContext(Response);
            if (m_Contexts[AssociationIndex].Validate(Response)) {
                m_pCurrentContext = &m_Contexts[AssociationIndex];
                m_pCurrentContext->m_ClientSAP = Response.m_DestinationAddress;
                m_pCurrentContext->m_ServerSAP = Response.m_SourceAddress;
                m_pCurrentContext->m_SecurityOptions.CallingAPTitle = Response.m_SecurityOptions.CallingAPTitle;
                m_pCurrentContext->m_SecurityOptions.CtoS = Response.m_SecurityOptions.CtoS;
                m_pCurrentContext->m_SecurityOptions.StoC = Response.m_SecurityOptions.StoC;
                m_pCurrentContext->m_xDLMS = Context;
                m_pCurrentContext->m_Initialized = true;
                //ASNObjectIdentifier     ApplicationContextName;
                //ASNObjectIdentifier     MechanismName;
                //AuthenticationValueType AuthenticationValue;
                //SecurityContext         SecurityContext;
                return true;
            }
        }
        return false;
    }

    bool Association::ReleaseAssociation(const APPReleaseConfirmOrResponse& Response)
    {
        if (m_pCurrentContext) {
            m_pCurrentContext->m_Initialized = false;
            m_pCurrentContext->m_ClientSAP = INVALID_ADDRESS;
        }
        m_pCurrentContext = nullptr;
        return true;
    }

    const AssociationContext* Association::GetAssociationContext(const COSEMAddressType& Address)
    {
        return GetAssociationContextByAddress(Address);
    }

    COSEMAddressType Association::GetAssociatedAddress() const
    {
        //
        // Only one at this point.  Phase II.
        //
        if (m_pCurrentContext)
            return m_pCurrentContext->m_ClientSAP;
        return INVALID_ADDRESS;
    }

    void Association::ReleaseTransientAssociations()
    {
        //
        // All are transient at this phase.
        //
        if (m_pCurrentContext) {
            m_pCurrentContext->m_Initialized = false;
            m_pCurrentContext->m_ClientSAP = INVALID_ADDRESS;
        }
        m_pCurrentContext = nullptr;
    }

    const AssociationContext* Association::GetAssociationContext(
        const APPBaseCallbackParameter& Parameter)
    {
        return GetAssociationContextByAddress(Parameter.m_SourceAddress);
    }

    ssize_t Association::FindObject(const Cosem_Attribute_Descriptor& Descriptor) const
    {
        ssize_t RetVal = 0;
        for (; RetVal < m_pObjects->size(); ++RetVal)
        {
            if (m_pObjects->at(RetVal)->Supports(Descriptor))
            {
                return RetVal;
            }
        }
        return -1;
    }

    APDUConstants::Data_Access_Result Association::InternalGet(const AssociationContext& Context,
        ICOSEMAttribute* pAttribute,
        const Cosem_Attribute_Descriptor& Descriptor,
        SelectiveAccess* pSelectiveAccess)
    {
        bool                              AppendResult = false;
        uint8_t                           AssociationIndex =
            Descriptor.instance_id.GetValueGroup(COSEMObjectInstanceID::VALUE_GROUP_E);
        AssociationContext* pContext;
        APDUConstants::Data_Access_Result RetVal = APDUConstants::Data_Access_Result::temporary_failure;
        //
        // Current Association?
        //
        if (0 == AssociationIndex)
        {
            pContext = GetAssociationContextByAddress(Context.m_ClientSAP);
            while (GetAssociationContextByIndex(AssociationIndex) != pContext)
            {
                ++AssociationIndex;
            }
        }
        else
        {
            pContext = GetAssociationContextByIndex(AssociationIndex);
        }
        if (!pContext)
        {
            return APDUConstants::Data_Access_Result::object_unavailable;
        }
        switch (pAttribute->AttributeID)
        {
        case ATTR_OBJ_LIST:     // ToDo - check LogocalDevice::m_Objects
        {
            LOG_ALL("HERE WE WERE\r\n");
            DLMSArray array;
            for (const AssociationConfigType& Entry : m_ObjectsByContext[AssociationIndex])
            {
                ssize_t Index = FindObject({ Entry.ClassID, {Entry.OID}, LOGICAL_NAME });
                if (Index >= 0)
                {
                    const ICOSEMObject* pObject = m_pObjects->at(Index);
                    const ICOSEMInterface* pInterface = dynamic_cast<const ICOSEMInterface*>(pObject);
                    array.push_back(DLMSStructure({
                        pInterface->GetClassID(),
                        pInterface->GetVersion(),
                        COSEMObjectInstanceID{Entry.OID},
                        pInterface->GetAccessRights(Entry.OID)
                    }));

                }
            }
            pAttribute->SelectChoice(COSEMDataType::NULL_DATA);
            AppendResult = pAttribute->Append(array);
            LOG_ALL("HERE WE ARE %d\r\n", AppendResult);
        }
        break;
        case ATTR_PARTNERS_ID:
            AppendResult = pAttribute->Append(
                DLMSStructure
                ({
                    pContext->m_ClientSAP,
                    pContext->m_ServerSAP,      // ToDo - change to uint16_t
                    }));
            break;
        case ATTR_APP_CTX_NAME:
            pAttribute->SelectChoice(COSEMDataType::OCTET_STRING);
            AppendResult = pAttribute->Append(pContext->m_SecurityOptions.ApplicationContextName);
            break;
        case ATTR_XDLMS_CTX_INFO:
            pAttribute->SelectChoice(COSEMDataType::STRUCTURE);
            AppendResult = pAttribute->Append(
                DLMSStructure
                ({
                    pContext->m_xDLMS.ConformanceBits(),
                    pContext->m_xDLMS.APDUSize(),
                    pContext->m_xDLMS.APDUSize(),
                    pContext->m_xDLMS.DLMSVersion(),
                    pContext->m_xDLMS.QOS(),
                    pContext->m_xDLMS.DedicatedKey()
                    }));
            break;
        case ATTR_AUTH_MECH_NAME:
            pAttribute->SelectChoice(COSEMDataType::OCTET_STRING);
            AppendResult = pAttribute->Append(pContext->m_SecurityOptions.MechanismName);
            break;
        case ATTR_SECRET:
            //
            // Not cool to have the SECRET out
            //
            RetVal = APDUConstants::Data_Access_Result::read_write_denied;
            break;
        case ATTR_STATUS:
            AppendResult = pAttribute->Append((uint8_t)pContext->m_Status);
            break;
        case ATTR_SECURITY_SETUP_REF:
            if (pContext->m_SecurityOptions.SecurityContext.GetSecuritySuite())
            {
                DLMSVector data = DLMSVector(pContext->m_SecurityOptions.SecurityContext.GetSecuritySetupObjectID());
                AppendResult = pAttribute->Append(data);
            }
            else
            {
                RetVal = APDUConstants::Data_Access_Result::object_unavailable;
            }
            break;
        default:
            RetVal = APDUConstants::Data_Access_Result::object_unavailable;
            break;
        }
        if (AppendResult)
        {
            RetVal = APDUConstants::Data_Access_Result::success;
        }
        return RetVal;
    }

    APDUConstants::Action_Result Association::InternalAction(const AssociationContext& Context,
        ICOSEMMethod* pMethod,
        const Cosem_Method_Descriptor& Descriptor,
        const DLMSOptional<DLMSVector>& Parameters,
        DLMSVector* pReturnValue /*= nullptr*/)
    {
        bool                              AppendResult = false;
        uint8_t                           AssociationIndex =
            Descriptor.instance_id.GetValueGroup(COSEMObjectInstanceID::VALUE_GROUP_E);
        AssociationContext* pContext;
        APDUConstants::Action_Result RetVal = APDUConstants::Action_Result::temporary_failure;
        //
        // Current Association?
        //
        if (0 == AssociationIndex)
        {
            pContext = GetAssociationContextByAddress(Context.m_ClientSAP);
            while (GetAssociationContextByIndex(AssociationIndex) != pContext)
            {
                ++AssociationIndex;
            }
        }
        else
        {
            pContext = GetAssociationContextByIndex(AssociationIndex);
        }
        if (!pContext)
        {
            return APDUConstants::Action_Result::object_unavailable;
        }
        switch (pMethod->MethodID)
        {
        case METHOD_REPLY_TO_HLS_AUTHENTICATION:
            if (pContext->m_SecurityOptions.MechanismName == COSEMSecurityOptions::MechanismNameHighLevelSecurity)
            {
                if (Parameters.value()[0] == 0)
                {
                    RetVal = APDUConstants::Action_Result::other_reason;
                    break;
                }
                if (pReturnValue)
                {
                    COSEMType Value(OctetStringSchema);
                    Value.Parse(DLMSVector(Parameters.value(), 1, Parameters.value().Size() - 1));
                    DLMSValue Val;
                    Value.GetNextValue(&Val);
                    DLMSVector StoC_Reply = DLMSValueGet<DLMSVector>(Val);

                    DLMSVector key;
                    if (pContext->m_SecurityOptions.AuthenticationType() == APDUConstants::AuthenticationValueChoice::charstring)
                    {
                        key.Append(DLMSValueGet<VISIBLE_STRING_CType>(pContext->m_SecurityOptions.AuthenticationValue));
                    }
                    else if (pContext->m_SecurityOptions.AuthenticationType() == APDUConstants::AuthenticationValueChoice::external)
                    {
                        key = DLMSValueGet<DLMSVector>(pContext->m_SecurityOptions.AuthenticationValue);
                    }
                    else
                    {
                        throw std::runtime_error("Authentication type not implemented");
                    }

                    DLMSVector CtoS;
                    if (pContext->m_SecurityOptions.CtoS.is<DLMSVector>())
                    {
                        CtoS = DLMSValueGet<DLMSVector>(pContext->m_SecurityOptions.CtoS);
                    }
                    else if (pContext->m_SecurityOptions.CtoS.is<VISIBLE_STRING_CType>())
                    {
                        CtoS.Append(DLMSValueGet<VISIBLE_STRING_CType>(pContext->m_SecurityOptions.CtoS));
                    }
                    else
                    {
                        throw std::runtime_error("CtoS type not implemented");
                    }

                    LibOpenSSL::AES1 aes1(key.GetData(), key.Size());

                    //{
                    //    uint8_t* stoc = new uint8_t[StoC_Reply.Size()];
                    //    aes1.Decrypt(StoC_Reply.GetData(), StoC_Reply.Size(), stoc);
                    //    DLMSVector StoC(stoc, StoC_Reply.Size());
                    //    delete[] stoc;

                    //    // ToDo - Himanshu - HLS
                    //    if (StoC == DLMSValueGet<DLMSVector>(pContext->m_SecurityOptions.StoC))
                    //    {
                    //        RetVal = APDUConstants::Action_Result::success;
                    //    }
                    //    else
                    //    {
                    //        RetVal = APDUConstants::Action_Result::other_reason;
                    //        break;
                    //    }
                    //}

                    {
                        DLMSVector StoC = DLMSValueGet<DLMSVector>(pContext->m_SecurityOptions.StoC);

                        uint8_t* stoc = new uint8_t[StoC_Reply.Size()];
                        aes1.Encrypt(StoC.GetData(), StoC.Size(), stoc);
                        DLMSVector StoC_Expected(stoc, StoC_Reply.Size());
                        delete[] stoc;

                        if (StoC_Expected == StoC_Reply)
                        {
                            RetVal = APDUConstants::Action_Result::success;
                        }
                        else
                        {
                            RetVal = APDUConstants::Action_Result::other_reason;
                            break;
                        }
                    }

                    uint8_t* ctos = new uint8_t[CtoS.Size()];
                    aes1.Encrypt(CtoS.GetData(), CtoS.Size(), ctos);
                    DLMSVector CtoS_Reply(ctos, CtoS.Size());
                    delete[] ctos;

                    COSEMType Data(COSEMDataType::OCTET_STRING, CtoS_Reply);
                    *pReturnValue = Data.GetBytes();
                    RetVal = APDUConstants::Action_Result::success;
                }
                else
                {
                    throw std::runtime_error("No return value");
                }
            }
            else if (pContext->m_SecurityOptions.MechanismName == COSEMSecurityOptions::MechanismNameHighLevelSecurityGMAC)
            {
                if (Parameters.value()[0] == 0)
                {
                    RetVal = APDUConstants::Action_Result::other_reason;
                    break;
                }
                if (pReturnValue)
                {
                    COSEMType Value(OctetStringSchema);
                    Value.Parse(DLMSVector(Parameters.value(), 1, Parameters.value().Size() - 1));
                    DLMSValue Val;
                    Value.GetNextValue(&Val);
                    DLMSVector StoC_Reply = DLMSValueGet<DLMSVector>(Val);

                    DLMSVector IV, Tag;

                    IV.Append(METER_SYSTEM_TITLE, 0, 0);
                    uint32_t IC = pContext->m_SecurityOptions.SecurityContext.GetSecuritySuite()->GetRandom();
                    IV.Append<uint32_t>(IC);
                    
                    // Todo - Himanshu - HLS
                    pContext->m_SecurityOptions.SecurityContext.GetSecuritySuite()->GenerateGMAC(
                        IV,
                        pContext->m_SecurityOptions.CtoS,
                        Tag
                    );
                    pReturnValue->Append<uint8_t>(pContext->m_SecurityOptions.SecurityContext.GetSecuritySuite()->GetSecurityControlByte());
                    pReturnValue->Append<uint32_t>(IC);
                    pReturnValue->Append(Tag);

                    COSEMType Data(COSEMDataType::OCTET_STRING, *pReturnValue);
                    *pReturnValue = Data.GetBytes();
                    RetVal = APDUConstants::Action_Result::success;
                }
                else
                {
                    throw std::runtime_error("No return value");
                }
            }
            else
            {
                RetVal = APDUConstants::Action_Result::type_unmatched;
            }
            break;
        default:
            RetVal = APDUConstants::Action_Result::object_unavailable;
            break;
        }
        return RetVal;
    }

    AssociationContext* Association::GetAssociationContextByIndex(int Index)
    {
        AssociationInfoList::iterator it = m_Contexts.begin();
        while (it != m_Contexts.end())
        {
            if (0 == Index--)
            {
                return &(*it);
            }
            ++it;
        }
        return nullptr;
    }

    AssociationContext* Association::GetAssociationContextByAddress(COSEMAddressType Address)
    {
        AssociationInfoList::iterator it =
            std::find_if(m_Contexts.begin(),
                m_Contexts.end(),
                [Address](const AssociationContext& Entry) -> bool
                {
                    return Address == Entry.m_ClientSAP;
                });
        if (it == m_Contexts.end())
        {
            return nullptr;
        }
        return &(*it);
    }
}