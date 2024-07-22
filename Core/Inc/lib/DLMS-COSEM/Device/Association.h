
#pragma once

#include "interfaces/IAssociationLN.h"
#include "COSEM/COSEMSecurity.h"

namespace EPRI
{
    struct APPBaseCallbackParameter;
    struct APPOpenRequestOrIndication;
    struct APPOpenConfirmOrResponse;
    struct APPReleaseRequestOrIndication;
    struct APPReleaseConfirmOrResponse;

    struct AssociationContext
    {
        using AssociationStatusType = IAssociationLN::AssociationState;

        AssociationContext();
        //AssociationContext(const APPOpenConfirmOrResponse& Response);
        AssociationContext& operator=(const AssociationContext& RHS);
        //AssociationContext
        // Partners
        //
        COSEMAddressType        m_ClientSAP;
        COSEMAddressType        m_ServerSAP;
        //
        // Application Context
        //
        COSEMSecurityOptions    m_SecurityOptions;
        //
        // xDLMS Context Information
        //
        xDLMS::Context          m_xDLMS;
        //
        // Status Information
        //
        AssociationStatusType   m_Status;

        bool m_Initialized = false;

        bool Validate(const APPOpenConfirmOrResponse& Response);
    };

    class Association : public IAssociationLNObject
    {
    public:
        Association() = delete;
        Association(std::vector<ICOSEMObject*>* pObjects);
        Association(std::vector<ICOSEMObject*>* pObjects
            , const COSEMObjectInstanceID& OID
            , uint16_t ShortNameBase = std::numeric_limits<uint16_t>::max());
        virtual ~Association();

        virtual size_t AvailableAssociations() const;
        virtual const std::shared_ptr<ISecuritySuite> GetSecuritySetup(COSEMAddressType Address) const;
        virtual bool RegisterAssociation(const APPOpenConfirmOrResponse& Response);
        virtual bool ReleaseAssociation(const APPReleaseConfirmOrResponse& Response);
        virtual const AssociationContext* GetAssociationContext(
            const APPBaseCallbackParameter& Parameter);
        COSEMAddressType GetAssociatedAddress() const;
        virtual void ReleaseTransientAssociations();

    protected:
        //using AssociationInfoList = std::list<AssociationContext>;
        using AssociationInfoList = std::vector<AssociationContext>;
        virtual APDUConstants::Data_Access_Result InternalGet(const AssociationContext& Context,
            ICOSEMAttribute* pAttribute,
            const Cosem_Attribute_Descriptor& Descriptor,
            SelectiveAccess* pSelectiveAccess) final;
        AssociationContext* GetAssociationContextByIndex(int Index);
        AssociationContext* GetAssociationContextByAddress(COSEMAddressType Address);
        ssize_t FindObject(const Cosem_Attribute_Descriptor& Descriptor) const;

        //static 
        AssociationInfoList m_Contexts;
        AssociationContext * m_pCurrentContext = nullptr;
        std::vector<ICOSEMObject *> * m_pObjects;
        std::vector<std::vector<AssociationConfigType>> m_ObjectsByContext;
    };
}