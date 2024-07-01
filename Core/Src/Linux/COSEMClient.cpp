#include <iostream>
#include <cstdio>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <iomanip>
#include <asio.hpp>
#include <algorithm>
#include <string>
#include <pthread.h>                    //sudeshna
#include <fstream>                      //sudeshna

#include "BaseLibrary.h"
#include "COSEMClient.h"

using namespace std;
using namespace EPRI;
using namespace asio;

extern const std::string packet_types[2];
extern std::vector<std::string> TCPAddress;                             // Himanshu

bool LinuxClientEngine::OnOpenConfirmation(COSEMAddressType ServerAddress)
{
	// Base()->GetDebug()->TRACE("\n\nAssociated with Server %d...\n\n",
	// 	ServerAddress);
	Base()->GetDebug()->TRACE("\n\nAssociated with Server %d...\n\n", m_meter_idx);
	return true;
}

bool LinuxClientEngine::OnGetConfirmation(RequestToken Token, const GetResponse& Response)
{
	Base()->GetDebug()->TRACE("\n\nGet Confirmation for Token %d...\n", Token);
	if (Response.ResultValid && Response.Result.which() == Get_Data_Result_Choice::data_access_result)
	{
		Base()->GetDebug()->TRACE("\tReturned Error Code %d...\n",
			Response.Result.get<APDUConstants::Data_Access_Result>());
		return false;
	}

	// Himanshu
	if (Response.Descriptor.attribute_id == ICOSEMInterface::LOGICAL_NAME)
	{
		COSEMAttribute<ICOSEMInterface::LOGICAL_NAME, OctetStringSchema, 0x00> logical_name;
		DLMSValue Value;
		logical_name = Response.Result.get<DLMSVector>();
		if (COSEMType::VALUE_RETRIEVED == logical_name.GetNextValue(&Value))
		{
			DLMSVector Data = DLMSValueGet<OCTET_STRING_CType>(Value);
			Base()->GetDebug()->TRACE("%d-%d:%d.%d.%d*%d\r\n\r\n"
				, Data[0], Data[1], Data[2], Data[3], Data[4], Data[5]);
			return true;
		}
		return false;
	}

	if (CLSID_IData == Response.Descriptor.class_id)
	{
		IData     SerialNumbers;
		DLMSValue Value;

		SerialNumbers.value = Response.Result.get<DLMSVector>();
		if (COSEMType::VALUE_RETRIEVED == SerialNumbers.value.GetNextValue(&Value))
		{
			Base()->GetDebug()->TRACE("%s\n", DLMSValueGet<VISIBLE_STRING_CType>(Value).c_str());
		}

	}

	// Himanshu
	else if (CLSID_IRegister == Response.Descriptor.class_id)
	{
		// TODO - accomodate individual register read requests
		LOG_INFO("Processing GET Request for class Register\n");

        uint8_t obis_vg_c = Response.Descriptor.instance_id.GetValueGroup(EPRI::COSEMObjectInstanceID::VALUE_GROUP_C);
		if (Response.Descriptor.attribute_id == IRegister::ATTR_VALUE) {
			IRegister Register;
			DLMSValue Value;
			DLMSVector Data = Response.Result.get<DLMSVector>();
			Register.value = Data;
			if (COSEMType::VALUE_RETRIEVED == Register.value.GetNextValue(&Value))
			{
#if LOGGING_AT_OR_BELOW(LOG_LVL_INFO)
				Base()->GetDebug()->TRACE("Getting data\r\n%f\r\n", DLMSValueGet<float>(Value));
#endif
			}
		}
		else if (Response.Descriptor.attribute_id == IRegister::ATTR_SCALAR_UNIT) {
			IRegister Register;
			DLMSValue Value;
			DLMSVector Data = Response.Result.get<DLMSVector>();
			Register.scalar_unit_type = Data;
			if (COSEMType::VALUE_RETRIEVED == Register.scalar_unit_type.GetNextValue(&Value))
			{
				DLMSStructure& S = DLMSValueGetStructure(Value);
				Base()->GetDebug()->TRACE("Scalar : %d, Unit : %u\r\n", DLMSValueGet<int8_t>(S[0]), DLMSValueGet<uint8_t>(S[1]));
			}
		}
		else {
			LOG_ERROR("Register attribute not implemnted\r\n");
		}
	}

#pragma region

	else if (CLSID_IAssociationLN == Response.Descriptor.class_id)
	{
		IAssociationLN CurrentAssociation;
		DLMSValue      Value;

		switch (Response.Descriptor.attribute_id)
		{
		case IAssociationLN::ATTR_PARTNERS_ID:
			{
				CurrentAssociation.associated_partners_id = Response.Result.get<DLMSVector>();
				if (COSEMType::VALUE_RETRIEVED == CurrentAssociation.associated_partners_id.GetNextValue(&Value) &&
					IsStructure(Value))
				{
					DLMSStructure& Element = DLMSValueGetStructure(Value);
					Base()->GetDebug()->TRACE("ClientSAP %d; ServerSAP %d\n",
						DLMSValueGet<INTEGER_CType>(Element[0]),
						DLMSValueGet<LONG_UNSIGNED_CType>(Element[1]));
				}
			}
			break;

		default:
			LOG_ERROR("Attribute %d not supported for parsing.", Response.Descriptor.attribute_id);
			break;
		}
	}
	return true;
}

bool LinuxClientEngine::GetResponseConfirmed(RequestToken Token)
{
	try{
		const GetResponse Response = PickupResponse<GetResponse>(Token);
		return Response.ResultValid;
	}
	catch(...)
	{
		return false;
	}
}

bool LinuxClientEngine::OnSetConfirmation(RequestToken Token, const SetResponse& Response)
{
	Base()->GetDebug()->TRACE("\n\nSet Confirmation for Token %d...\n", Token);
	if (Response.ResultValid)
	{
		Base()->GetDebug()->TRACE("\tResponse Code %d...\n", Response.Result);

		if (CLSID_IRegister == Response.Descriptor.class_id)
		{
			switch (Response.Descriptor.attribute_id)
			{
			default:
				break;
			}
		}
	}
	return true;
}

bool LinuxClientEngine::OnActionConfirmation(RequestToken Token, const ActionResponse& Response)
{
	Base()->GetDebug()->TRACE("\n\nAction Confirmation for Token %d...\n", Token);
	if (Response.ResultValid)
	{
		Base()->GetDebug()->TRACE("\tResponse Code %d...\n",
			Response.Result);
	}
	return true;
}

bool LinuxClientEngine::OnReleaseConfirmation()
{
	Base()->GetDebug()->TRACE("\n\nRelease Confirmation from Server\n\n");
	return true;
}

bool LinuxClientEngine::OnReleaseConfirmation(COSEMAddressType ServerAddress)
{
	Base()->GetDebug()->TRACE("\n\nRelease Confirmation from Server %d\n\n", ServerAddress);
	return true;
}

bool LinuxClientEngine::OnAbortIndication(COSEMAddressType ServerAddress)
{
	if (INVALID_ADDRESS == ServerAddress)
	{
		Base()->GetDebug()->TRACE("\n\nAbort Indication.  Not Associated.\n\n");
		Base()->GetDebug()->TRACE("====================================\n\n");
	}
	else
	{
		// Base()->GetDebug()->TRACE("\n\nAbort Indication from Server %d\n\n", ServerAddress);
		Base()->GetDebug()->TRACE("\n\nAbort Indication from Server\n\n");
		Base()->GetDebug()->TRACE("====================================\n\n");
	}
	return true;
}

// Himanshu - ACCESS
bool LinuxClientEngine::OnAccessConfirmation(RequestToken Token, const AccessResponse& Response)
{
	Base()->GetDebug()->TRACE("\n\nAccess Confirmation for Token %d...\n", Token);
	if (Response.ResultValid)
	{
		COSEMClientEngine::OnAccessConfirmation(Token, Response);
	}
	return true;
}

void LinuxClientEngine::UpdateAddress(COSEMAddressType ClientAddress)
{
	m_Client.UpdateAddress(ClientAddress);
}   // Himanshu