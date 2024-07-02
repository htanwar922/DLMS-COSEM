#include <iostream>
#include <cstdio>
#include <time.h>
#include <ctype.h>
#include <iomanip>
#include <asio.hpp>
#include <algorithm>
#include <string>
#include <fstream>                      //sudeshna

#include "config.h"

#include "BaseLibrary.h"
#include "Apps.h"

#include "HDLCLLC.h"
#include "COSEM/COSEM.h"
#include "wrapper/SerialWrapper.h"
#include "wrapper/TCPWrapper.h"

using namespace std;
using namespace EPRI;
using namespace asio;

bool compressed = true;
vector<std::string> TCPAddress;                             // Himanshu

ClientApp::ClientApp(LinuxBaseLibrary& BL, int clients/*=0*/, const char** addresses/*=NULL*/)
	: AppBase(BL), m_ReadTimer(m_Base.get_io_service()) //sudeshna m_ReadTimer part
{
	m_Base.get_io_service().post(std::bind(&ClientApp::ClientMenu, this));
	// ReadLine(std::bind(&ClientApp::ClientMenu_Handler, this, std::placeholders::_1));    // Himanshu

	if(clients == 0)
		TCPAddress.push_back("localhost");
	for(int i=0; i<clients; i++)
		TCPAddress.push_back(addresses[i]);

	// ClientMenu_Handler("7"); // Himanshu Sudeshna
}

#pragma region // Himanshu
void ClientApp::open_func(std::string Address)
{

	if(m_pClientEngine == nullptr)
	{
		int SourceAddress = 0;
		m_pSocket = Base()->GetCore()->GetIP()->CreateSocket(LinuxIP::Options(LinuxIP::Options::MODE_CLIENT));
		m_pXPort = new TCPWrapper(m_pSocket);
		m_pClientEngine = new LinuxClientEngine(COSEMClientEngine::Options(SourceAddress), m_pXPort);
	}
	else
	{
		// TODO - update address
		m_pClientEngine->UpdateAddress(m_pClientEngine->m_meter_idx);
	}

	Base()->GetDebug()->TRACE("Connecting to Server %d\n", m_pClientEngine->m_meter_idx);

	while(m_pSocket->Open(Address.c_str()) != SUCCESSFUL); /*= DEFAULT_DLMS_PORT*/
	m_ReadTimer.expires_from_now(std::chrono::seconds(1));
	m_ReadTimer.async_wait(std::bind(&ClientApp::associate_func, this));
}

void ClientApp::associate_func()
{
	if (m_pSocket && m_pSocket->IsConnected() & m_pClientEngine->IsTransportConnected())
	{
		bool                 Send = true;
		int                  DestinationAddress = 1;
		COSEMSecurityOptions::SecurityLevel Security = (COSEMSecurityOptions::SecurityLevel) COSEMSecurityOptions::SECURITY_NONE;
		std::string          Password;
		COSEMSecurityOptions SecurityOptions;
		//
		// Only supports LN at this time
		//
		SecurityOptions.ApplicationContextName = SecurityOptions.ContextLNRNoCipher;

		if (Send)
		{
			size_t APDUSize =  640;
			m_pClientEngine->Open(DestinationAddress,
								SecurityOptions,
								xDLMS::InitiateRequest(APDUSize));
		}

		m_ReadTimer.expires_from_now(std::chrono::seconds(1));
		m_ReadTimer.async_wait(std::bind(&ClientApp::polling_func,
			this,
			m_waiting_interval));
	}
	else
	{
		PrintLine("Transport Connection Not Established Yet!\n");
		m_ReadTimer.expires_from_now(std::chrono::seconds(1));
		m_ReadTimer.async_wait(std::bind(&ClientApp::close_func,
			this,
			false));
	}
}

void ClientApp::polling_func(int interval/*=0*/)   // const asio::error_code& Error,
{
	Base()->GetDebug()->TRACE("Poll\n");

	Cosem_Attribute_Descriptor Descriptor;
	Descriptor.class_id = CLASS_ID;
	Descriptor.attribute_id = 2;
	if (Descriptor.instance_id.Parse(compressed ? configM_CONTINUOUS_OBIS_CODE_COMPRESSED : configM_CONTINUOUS_OBIS_CODE_ORIGINAL))
	{
		compressed = not compressed;
		if (m_pClientEngine->Get(Descriptor, &m_GetToken))
		{
			PrintLine(std::string("\tGet Request Sent: Token ") + std::to_string(m_GetToken) + "\n");
		}

		// TODO #1 - m_pClientEngine->m_Responses for m_GetToken in signal_handler or ClientHandler("0");
	}
	if (m_continuous_poll)
	{
		m_ReadTimer.expires_from_now(std::chrono::seconds(m_pClientEngine->m_meter_idx != TCPAddress.size()-1 ? 1 : interval));
		m_ReadTimer.async_wait(std::bind(&ClientApp::await_response_func,
			this,
			m_GetToken));
	}
}

void ClientApp::await_response_func(EPRI::COSEMClientEngine::RequestToken Token)
{
	Base()->GetDebug()->TRACE("Wait\n");

	if(m_pSocket && m_pSocket->IsConnected() && m_pClientEngine->IsTransportConnected())
	{
		if(! m_pClientEngine->GetResponseConfirmed(Token))
		{
			m_ReadTimer.expires_from_now(std::chrono::seconds(1));
			m_ReadTimer.async_wait(std::bind(&ClientApp::await_response_func,
				this,
				Token));
		}
		else
		{
			m_ReadTimer.expires_from_now(std::chrono::seconds(1));
			m_ReadTimer.async_wait(std::bind(&ClientApp::close_func,
				this,
				false));
		}
	}
	else
	{
		PrintLine("Transport Connection Broken!\n");
		m_ReadTimer.expires_from_now(std::chrono::seconds(1));
		m_ReadTimer.async_wait(std::bind(&ClientApp::close_func,
			this,
			false));
	}
}

void ClientApp::close_func(bool exit_on_close)
{
	Base()->GetDebug()->TRACE("Closed connection with Server %d\n", m_pClientEngine->m_meter_idx);

	m_pSocket->Close();
	m_pClientEngine->m_meter_idx = (m_pClientEngine->m_meter_idx + 1) % TCPAddress.size();

	m_ReadTimer.expires_from_now(std::chrono::seconds(1));
	if(exit_on_close) m_ReadTimer.async_wait(std::bind(&exit, 0));
	else m_ReadTimer.async_wait(std::bind(&ClientApp::open_func,
			this,
			TCPAddress[m_pClientEngine->m_meter_idx]));
}

void ClientApp::set_polling_interval(int seconds)
{
	m_waiting_interval = seconds;
}

#pragma endregion // Himanshu

void ClientApp::ClientMenu()
{
	PrintLine("\nClient Menu:\n\n");
	PrintLine("\t0 - Exit Application\n");
	PrintLine("\tA - TCP Connect\n");
	PrintLine("\tB - HDLC Physical Connect\n");
	PrintLine("\tC - HDLC Identify\n");
	PrintLine("\tD - HDLC Datalink Connect\n");
	PrintLine("\tE - Serial Wrapper Connect\n");
	PrintLine("\n");
	PrintLine("\t1 - COSEM Open\n");
	PrintLine("\t2 - COSEM Get\n");
	PrintLine("\t3 - COSEM Set\n");
	PrintLine("\t4 - COSEM Action\n");
	PrintLine("\t5 - COSEM Release\n");
	PrintLine("\t6 - COSEM ACCESS");
	// PrintLine("\t7 - COSEM Continuous Get Start\n");
	// PrintLine("\t8 - COSEM Continuous Get Stop\n");
	PrintLine("\n");
	PrintLine("\tT - TCP Disconnect\n");
	PrintLine("\tU - HDLC Disconnect\n");
	PrintLine("\tV - Serial Wrapper Disconnect\n");
	PrintLine("\nSelect: ");
	ReadLine(std::bind(&ClientApp::ClientMenu_Handler, this, std::placeholders::_1));
}

bool ClientApp::IdentifyResponse(const BaseCallbackParameter& Parameters)
{
	const DLIdentifyResponseParameter& Response =
		dynamic_cast<const DLIdentifyResponseParameter&>(Parameters);
	Base()->GetDebug()->TRACE("\n\nIdentify Response (%d, %d, %d, %d)...\n\n",
		Response.SuccessCode, Response.ProtocolID, Response.ProtocolVersion,
		Response.ProtocolRevision);
	return true;
}

bool ClientApp::DisconnectConfirm(const BaseCallbackParameter& Parameters)
{
	const DLDisconnectConfirmOrResponse& Response =
		dynamic_cast<const DLDisconnectConfirmOrResponse&>(Parameters);
	Base()->GetDebug()->TRACE("\n\nDisconnect Response from Server %d...\n\n",
		Response.DestinationAddress.LogicalAddress());

	Base()->GetCore()->GetSerial()->ReleaseSocket(m_pSerialSocket);
	m_pSerialSocket = nullptr;
	m_pSocket = nullptr;
	PrintLine("Serial released.");

	return true;
}

// Himanshu
std::string ClientApp::GetInstanceIDOrDefault(ClassIDType ClassID, ObjectAttributeIdType AttributeID, std::string Pre /*= ""*/)
{
	switch (ClassID)
	{
	case CLSID_IData:
		return GetStringInput(Pre + "OBIS Code (Default: 0-0:96.1.0*255)", "0-0:96.1.0*255");
	case CLSID_IRegister:
		switch (AttributeID)
		{
		case IRegister::ATTR_VALUE:
		case IRegister::ATTR_SCALAR_UNIT:
			return GetStringInput(Pre + "OBIS Code (Default: 1-0:0.7.0*255)", "1-0:0.7.0*255");
		}
	case CLSID_IClock:
		return GetStringInput(Pre + "OBIS Code (Default: 0-0:1.0.0*255)", "0-0:1.0.0*255");
	case CLSID_IAssociationLN:
		return GetStringInput(Pre + "OBIS Code (Default: 0-0:40.0.0*255)", "0-0:40.0.0*255");
	default:
		return GetStringInput(Pre + "OBIS Code (Default: 0-0:0.0.0*0)", "0-0:0.0.0*0");
	}
}

void ClientApp::ClientMenu_Handler(const std::string& RetVal)
{
	if (RetVal == "0")
	{
		m_ReadTimer.expires_from_now(std::chrono::seconds(1));
		m_ReadTimer.async_wait(std::bind(&ClientApp::await_response_func,
			this,
			m_GetToken));
		exit(0);
	}
	else if (toupper(RetVal[0]) == 'A')
	{
		int         SourceAddress = GetNumericInput("Client Address (Default: 1)", 1);
		std::string TCPAddress = GetStringInput("Destination TCP Address (Default: localhost)", "localhost");
		int 		Port = GetNumericInput("Port (Default: 4059)", ISocket::DEFAULT_DLMS_PORT);

		m_pClientEngine = new LinuxClientEngine(COSEMClientEngine::Options(SourceAddress),
			new TCPWrapper((m_pSocket = Base()->GetCore()->GetIP()->CreateSocket(LinuxIP::Options(LinuxIP::Options::MODE_CLIENT)))));
		if (SUCCESSFUL != m_pSocket->Open(TCPAddress.c_str(), Port))
		{
			PrintLine("Failed to initiate connect\n");
		}
	}
	else if (toupper(RetVal[0]) == 'B')
	{
		int         SourceAddress = GetNumericInput("Client Address (Default: 1)", 1);
		std::string SerialPort = GetStringInput("Serial Port (Default: /dev/ttyUSB0)", "/dev/ttyUSB0");

		PrintLine("Initial Baud Rate Selection\n");
		PrintLine("\t6  - 9600\n");
		PrintLine("\t10 - 115200\n");
		int         BaudRate = GetNumericInput("(Default: 10 - 115200)", 10);
		//
		// Engine
		//
		m_pClientEngine = new LinuxClientEngine(COSEMClientEngine::Options(SourceAddress),
			(m_pHDLC = new HDLCClientLLC(HDLCAddress(SourceAddress),
				(m_pSerialSocket =
					Base()->GetCore()->GetSerial()->CreateSocket(LinuxSerial::Options(ISerial::Options::BaudRate(BaudRate)))),
				HDLCOptions())));
		//
		// Physical
		//
		if (m_pSerialSocket->Open(SerialPort.c_str()) != SUCCESS)
		{
			PrintLine("Failed to initiate connect\n");
		}
		else
		{
			m_pSocket = m_pSerialSocket;
		}
	}
	else if (toupper(RetVal[0]) == 'C')
	{
		if (m_pHDLC)
		{
			//
			// IDENTIFY
			//
			m_pHDLC->RegisterIdentifyConfirm(std::bind(&ClientApp::IdentifyResponse,
				this, std::placeholders::_1));
			m_pHDLC->IdentifyRequest(DLIdentifyRequestParameter(HDLCAddress()));
		}
		else
		{
			PrintLine("Physical Connection Not Established Yet!\n");
		}

	}
	else if (toupper(RetVal[0]) == 'D')
	{
		if (m_pSerialSocket && m_pHDLC && m_pSerialSocket->IsConnected())
		{
			int DestinationAddress = GetNumericInput("Destination Address (Default: 1)", 1);
			//
			// Datalink
			//
			m_pHDLC->ConnectRequest(DLConnectRequestOrIndication(HDLCAddress(DestinationAddress)));
		}
		else
		{
			PrintLine("Physical Connection Not Established Yet!\n");
		}
	}
	else if (toupper(RetVal[0]) == 'E')
	{
		int         SourceAddress = GetNumericInput("Client Address (Default: 1)", 1);
		std::string SerialPort = GetStringInput("Serial Port (Default: /dev/ttyUSB0)", "/dev/ttyUSB0");

		PrintLine("Initial Baud Rate Selection\n");
		PrintLine("\t6  - 9600\n");
		PrintLine("\t10 - 115200\n");
		int         BaudRate = GetNumericInput("(Default: 10 - 115200)", 10);

		m_pClientEngine = new LinuxClientEngine(COSEMClientEngine::Options(SourceAddress),
				new SerialWrapper(
					(m_pSerialSocket =
						Base()->GetCore()->GetSerial()->CreateSocket(LinuxSerial::Options(ISerial::Options::BaudRate(BaudRate))))));
		if (m_pSerialSocket->Open(SerialPort.c_str()) != SUCCESS)
		{
			PrintLine("Failed to initiate connect\n");
		}
		m_pSocket = m_pSerialSocket;

	}
	else if (RetVal == "1")
	{
		if (m_pSocket && m_pSocket->IsConnected() & m_pClientEngine->IsTransportConnected())
		{
			bool                 Send = true;
			int                  DestinationAddress = GetNumericInput("Server Address (Default: 1)", 1);
			COSEMSecurityOptions::SecurityLevel Security = (COSEMSecurityOptions::SecurityLevel)
				GetNumericInput("Security Level [0 - None, 1 - Low, 2 - High] (Default: 0)", COSEMSecurityOptions::SECURITY_NONE);
			std::string          Password;
			COSEMSecurityOptions SecurityOptions;
			//
			// Only supports LN at this time
			//
			SecurityOptions.ApplicationContextName = SecurityOptions.ContextLNRNoCipher;
			switch (Security)
			{
			case COSEMSecurityOptions::SECURITY_NONE:
				break;

			case COSEMSecurityOptions::SECURITY_LOW_LEVEL:
				SecurityOptions.MechanismName = SecurityOptions.MechanismNameLowLevelSecurity;
				SecurityOptions.AuthenticationValue = GetStringInput("Password", "");
				break;

			case COSEMSecurityOptions::SECURITY_HIGH_LEVEL:
			default:
				Send = false;
				PrintLine("Security Level is Not Supported at This Time");
				break;
			}
			if (Send)
			{
				size_t APDUSize = GetNumericInput("APDU Size (Default: 640)", 640);
				m_pClientEngine->Open(DestinationAddress,
										SecurityOptions,
										xDLMS::InitiateRequest(APDUSize));
			}
		}
		else
		{
			PrintLine("Transport Connection Not Established Yet!\n");
		}
	}
	else if (RetVal == "2")
	{
		if (m_pSocket && m_pSocket->IsConnected() && m_pClientEngine->IsOpen())
		{
			Cosem_Attribute_Descriptor Descriptor;

			Descriptor.class_id = (ClassIDType) GetNumericInput("Class ID (Default: 3)", CLSID_IRegister);
			Descriptor.attribute_id = (ObjectAttributeIdType) GetNumericInput("Attribute (Default: 2)", IRegister::ATTR_VALUE);
			if (Descriptor.instance_id.Parse(GetInstanceIDOrDefault(Descriptor.class_id, Descriptor.attribute_id)))
			{
				if (m_pClientEngine->Get(Descriptor,
											&m_GetToken))
				{
					PrintLine(std::string("\tGet Request Sent: Token ") + std::to_string(m_GetToken) + "\n");
				}
			}
			else
			{
				PrintLine("Malformed OBIS Code!\n");
			}
		}
		else
		{
			PrintLine("Not Connected!\n");
		}

	}
	else if (RetVal == "3")
	{
		if (m_pSocket && m_pSocket->IsConnected() && m_pClientEngine->IsOpen())
		{
			Cosem_Attribute_Descriptor Descriptor;

			Descriptor.class_id = (ClassIDType) GetNumericInput("Class ID (Default: 3)", CLSID_IRegister);
			Descriptor.attribute_id = (ObjectAttributeIdType) GetNumericInput("Attribute (Default: 2)", IRegister::ATTR_VALUE);
			if (Descriptor.instance_id.Parse(GetInstanceIDOrDefault(Descriptor.class_id, Descriptor.attribute_id)))
			{
				bool ret = false;
				switch (Descriptor.class_id)
				{
				case CLSID_IData: {
					COSEMType Data(COSEMDataType::VISIBLE_STRING, GetStringInput("Value (Default: LINUXDATA)", "LINUXDATA#"));
					ret = m_pClientEngine->Set(Descriptor, Data, &m_SetToken);
					break;
				}
				case CLSID_IRegister: {
					switch (Descriptor.attribute_id)
					{
					default:
						PrintLine("Attribute ID not supported!\n");
						break;
					}
					break;
				}
				default:
					PrintLine("Class ID not supported!\n");
					break;
				}
				if (ret)
				{
					PrintLine(std::string("\tSet Request Sent: Token ") + std::to_string(m_SetToken) + "\n");
				}
			}
			else
			{
				PrintLine("Malformed OBIS Code!\n");
			}
		}
		else
		{
			PrintLine("Not Connected!\n");
		}

	}
	else if (RetVal == "4")
	{
		if (m_pSocket && m_pSocket->IsConnected() && m_pClientEngine->IsOpen())
		{
			Cosem_Method_Descriptor Descriptor;

			Descriptor.class_id = (ClassIDType) GetNumericInput("Class ID (Default: 8)", CLSID_IClock);
			Descriptor.method_id = (ObjectAttributeIdType) GetNumericInput("Method (Default: 1)", 1);
			if (Descriptor.instance_id.Parse(GetInstanceIDOrDefault(Descriptor.class_id, Descriptor.method_id)))
			{
				COSEMType MyData(COSEMDataType::INTEGER, GetNumericInput("Parameter (Default: 1)", 1));
				if (m_pClientEngine->Action(Descriptor,
										DLMSOptional<DLMSVector>(MyData),
										&m_ActionToken))
				{
					PrintLine(std::string("\tAction Request Sent: Token ") + std::to_string(m_ActionToken) + "\n");
				}
			}
			else
			{
				PrintLine("Malformed OBIS Code!\n");
			}
		}
		else
		{
			PrintLine("Not Connected!\n");
		}

	}
	else if (RetVal == "5")
	{
		if (!m_pClientEngine->Release(xDLMS::InitiateRequest()))
		{
			PrintLine("Problem submitting COSEM Release!\n");
		}
	}
	// Himanshu - ACCESS
	else if (RetVal == "6")
	{
		if (m_pSocket && m_pSocket->IsConnected() && m_pClientEngine->IsOpen())
		{
			APPAccessRequestOrIndication::AccessRequestSpecs Specs;
			APPAccessRequestOrIndication::AccessRequestDatas Datas;
			Base()->GetDebug()->TRACE("\r\nACCESS Request Types:\r\n"
				"|-> GET: %d\r\n"
				"|-> SET: %d\r\n"
				"|-> ACTION: %d\r\n"
				"|-> GET-WITH-SELECTION: %d\r\n"
				"|-> SET-WITH-SELECTION: %d\r\n\r\n",
				Access_Request::access_request_get,
				Access_Request::access_request_set,
				Access_Request::access_request_action,
				Access_Request::access_request_get_with_selection,
				Access_Request::access_request_set_with_selection
			);
			int num = GetNumericInput(L_GREEN "Number of Requests (Default: 1)" L_RESET, 1);
			for (int i=0; i<num; i++) {
				Base()->GetDebug()->TRACE("|\r\n|-> " L_ITALIC L_UNDERLINE L_RED "ACCESS REQUEST SPECIFICATION #%d" L_RESET "\r\n", i + 1);
				Access_Request::AccessRequestSpecification Spec;
				Spec.m_Choice = (Access_Request::Access_Request_Choice) GetNumericInput("|" L_YELLOW "   |-> Request type (Default: GET(1))" L_RESET, Access_Request::access_request_get);
				Spec.m_Descriptor.class_id = (ClassIDType) GetNumericInput("|       |-> Class ID (Default: 3)", CLSID_IRegister);
				Spec.m_Descriptor.attribute_id = (ObjectAttributeIdType) GetNumericInput("|       |-> Attribute (Default: 2)", IRegister::ATTR_VALUE);
				if (Spec.m_Descriptor.instance_id.Parse(GetInstanceIDOrDefault(Spec.m_Descriptor.class_id, Spec.m_Descriptor.attribute_id, "|       |-> ")))
				{
					switch (Spec.m_Choice) {
					case Access_Request::access_request_get:
						Specs.push_back(Spec);
						Datas.push_back(DLMSVector{(uint8_t)COSEMDataType::NULL_DATA});
						break;
					default:
						PrintLine(std::string("Access Request type ") + std::to_string(Spec.m_Choice) + " not supported yet.\n");
						i--;
					}
				}
				else
				{
					PrintLine("Malformed OBIS Code!\n");
					i--;
				}
			}
			if (m_pClientEngine->Access(Specs,
										Datas,
										&m_AccessToken))
			{
				PrintLine(std::string("\tGet Request Sent: Token ") + std::to_string(m_GetToken) + "\n");
			}
		}
		else
		{
			PrintLine("Not Connected!\n");
		}

	}
	/*sudeshna's code starts here*/
	else if (RetVal == "7")
	{
		// ClientMenu_Handler("A");
		m_continuous_poll = true;
		open_func(TCPAddress[0]);
		// auto fun = std::bind(&ClientApp::polling_func,this, std::placeholders::_1, m_waiting_interval);
		// fun(Error);
	}
	else if (RetVal == "8")
	{
		m_ReadTimer.cancel();
		m_continuous_poll = false;
	}
	/*ends here*/
	else if (toupper(RetVal[0]) == 'T')
	{
		if (m_pSocket)
		{
			Base()->GetCore()->GetIP()->ReleaseSocket(m_pSocket);
			m_pSocket = nullptr;
			PrintLine("Socket released.\n");
		}
		else
		{
			PrintLine("TCP Not Opened!\n");
		}
	}
	else if (toupper(RetVal[0]) == 'U')
	{
		if (m_pHDLC && m_pHDLC->IsConnected())
		{
			m_pHDLC->RegisterDisconnectConfirm(std::bind(&ClientApp::DisconnectConfirm,
				this,
				std::placeholders::_1));
			m_pHDLC->DisconnectRequest(DLDisconnectRequestOrIndication());
		}
		else
		{
			PrintLine("HDLC Not Connected!\n");
		}
	}

	m_Base.get_io_service().post(std::bind(&ClientApp::ClientMenu, this));
}

#pragma region

ServerApp::ServerApp(LinuxBaseLibrary& BL) :
	AppBase(BL)
{
	m_Base.get_io_service().post(std::bind(&ServerApp::ServerMenu, this));
	ServerMenu_Handler("1");
}

void ServerApp::ServerMenu()
{
	PrintLine("\nServer Menu:\n\n");
	PrintLine("\t0 - Exit Application\n");
	PrintLine("\t1 - TCP Server\n");
	PrintLine("\t2 - HDLC Server\n");
	PrintLine("\t3 - Serial Wrapper Server\n\n");
	PrintLine("Select: ");
	ReadLine(std::bind(&ServerApp::ServerMenu_Handler, this, std::placeholders::_1));
}

void ServerApp::ServerMenu_Handler(const std::string& RetVal)
{
	if (RetVal == "0")
	{
		exit(0);
	}
	else if (RetVal == "1")
	{
		LinuxTCPSocket *   pSocket;
		PrintLine("\nTCP Server Mode - Listening on Port 4059\n");

		if(m_pServerEngine == nullptr)		// Himanshu
		{
			m_pServerEngine = new LinuxCOSEMServerEngine(COSEMServerEngine::Options(),
				new TCPWrapper((pSocket = (LinuxTCPSocket *)Base()->GetCore()->GetIP()->CreateSocket(LinuxIP::Options()))));
			if (SUCCESSFUL != pSocket->Open())
				PrintLine("Failed to initiate listen\n");
			else
				m_pSocket = pSocket;		// Himanshu
		}
		else	// Himanshu
		{
			m_pSocket->Close();
			if (SUCCESSFUL != m_pSocket->Accept())
				PrintLine("Failed to initiate listen\n");
		}
	}
	else if (RetVal == "2")
	{
		ISerialSocket * pSocket;
		int             ServerAddress = GetNumericInput("Server Address (Default: 1)", 1);
		std::string     SerialPort = GetStringInput("Serial Port (Default: /tmp/ttyS10)", "/tmp/ttyS10");

		PrintLine("Initial Baud Rate Selection\n");
		PrintLine("\t6  - 9600\n");
		PrintLine("\t10 - 115200\n");
		int         BaudRate = GetNumericInput("(Default: 6 - 9600)", 6);

		PrintLine(std::string("\nHDLC Server Mode - Listening on ") + SerialPort + std::string("\n"));
		//
		// TODO - HDLCServerLLC ServerAddress should be able to handle multiple SAPs
		//
		m_pServerEngine = new LinuxCOSEMServerEngine(COSEMServerEngine::Options(),
			new HDLCServerLLC(HDLCAddress(ServerAddress),
				(pSocket = Base()->GetCore()->GetSerial()->CreateSocket(LinuxSerial::Options(ISerial::Options::BaudRate(BaudRate)))),
			HDLCOptions()));

		if (pSocket->Open(SerialPort.c_str()) != SUCCESS)
		{
			PrintLine("Failed to initiate connect\n");
		}
	}
	else if (RetVal == "3")
	{
		ISerialSocket * pSocket;
		std::string     SerialPort = GetStringInput("Serial Port (Default: /tmp/ttyS10)", "/tmp/ttyS10");

		PrintLine("Initial Baud Rate Selection\n");
		PrintLine("\t6  - 9600\n");
		PrintLine("\t10 - 115200\n");
		int         BaudRate = GetNumericInput("(Default: 6 - 9600)", 6);

		PrintLine(std::string("\nSerial Wrapper Server Mode - Listening on ") + SerialPort + std::string("\n"));
		m_pServerEngine = new LinuxCOSEMServerEngine(COSEMServerEngine::Options(),
			new SerialWrapper((pSocket = Base()->GetCore()->GetSerial()->CreateSocket(LinuxSerial::Options(ISerial::Options::BaudRate(BaudRate))))));

		if (pSocket->Open(SerialPort.c_str()) != SUCCESS)
		{
			PrintLine("Failed to initiate connect\n");
		}
	}

	m_Base.get_io_service().post(std::bind(&ServerApp::ServerMenu, this));
}

#pragma endregion