#pragma once

#include "COSEM/COSEM.h"
#include "COSEM/COSEMDevice.h"
#include "COSEM/COSEMEngine.h"
#include "interfaces/IData.h"
#include "interfaces/IClock.h"
#include "interfaces/IRegister.h"

using namespace std;
using namespace EPRI;
using namespace asio;

class LinuxClientEngine : public COSEMClientEngine
{
public:
    LinuxClientEngine() = delete;
    LinuxClientEngine(const Options& Opt, Transport * pXPort)
        : COSEMClientEngine(Opt, pXPort)
    {
    }
    virtual ~LinuxClientEngine()
    {
    }

    virtual bool OnOpenConfirmation(COSEMAddressType ServerAddress);
    virtual bool OnGetConfirmation(RequestToken Token, const GetResponse& Response);
    virtual bool OnSetConfirmation(RequestToken Token, const SetResponse& Response);
    virtual bool OnActionConfirmation(RequestToken Token, const ActionResponse& Response);
    virtual bool OnReleaseConfirmation();
    virtual bool OnReleaseConfirmation(COSEMAddressType ServerAddress);
    virtual bool OnAbortIndication(COSEMAddressType ServerAddress);

    // Himanshu - ACCESS
    virtual bool OnAccessConfirmation(RequestToken Token, const AccessResponse& Response);

    virtual bool GetResponseConfirmed(RequestToken Token);  // Himanshu - Continuous GET
    virtual void UpdateAddress(COSEMAddressType ClientAddress); // Himanshu
    int m_meter_idx = 0;
};