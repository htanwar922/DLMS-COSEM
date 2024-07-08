#pragma once

#include "BaseLibrary.h"
#include "COSEMServer.h"
#include "COSEMClient.h"

#include "HDLCLLC.h"
#include "wrapper/SerialWrapper.h"
#include "wrapper/TCPWrapper.h"

#include "config.h"

using namespace std;
using namespace EPRI;
using namespace asio;

#define CLASS_ID 3 // 1
#define CONTINUOUS_OBIS_CODE "1-0:100.7.0*255" // "0-0:96.1.0*255"

class AppBase
{
    asio::io_service&                 m_IO;
public:
    typedef std::function<void(const std::string&)> ReadLineFunction;

    AppBase(LinuxBaseLibrary& BL) :
        m_Base(BL), m_IO(BL.get_io_service()),
        m_Input(BL.get_io_service(), dup(STDIN_FILENO)),
        m_Output(BL.get_io_service(), dup(STDOUT_FILENO))
    {
    }

    virtual void Run()
    {
        m_Base.Process();
    }

    virtual void PrintLine(const std::string& Line)
    {
#ifndef _MSC_VER
        asio::write(m_Output, asio::buffer(Line));
#else
        printf("%s", Line.c_str());
#endif // !_MSC_VER
    }

    virtual void ReadLine(ReadLineFunction Handler)
    {
        asio::async_read_until(m_Input,
            m_InputBuffer,
            '\n',
            std::bind(&AppBase::ReadLine_Handler,
                      this,
                      std::placeholders::_1,
                      std::placeholders::_2,
                      Handler));

    }

    virtual std::string GetLine()
    {
        asio::read_until(m_Input, m_InputBuffer, '\n');
        return ConsumeStream();
    }

protected:
    void ReadLine_Handler(const asio::error_code& Error, size_t BytesTransferred, ReadLineFunction Handler)
    {
        if (!Error)
        {
            Handler(ConsumeStream());
        }
    }

    std::string ConsumeStream()             // CHECK_LATER Himanshu
    {
        std::istream Stream(&m_InputBuffer);
        std::string  RetVal;
        std::getline(Stream, RetVal);
        return RetVal;
    }

    int GetNumericInput(const std::string& PromptText, int Default)
    {
        std::string RetVal;
        do
        {
            PrintLine(PromptText + ": ");
            RetVal = GetLine();
            try
            {
                if (RetVal.length())
                    return std::stoi(RetVal, nullptr, 0);
                else
                    return Default;
            }
            catch (const std::invalid_argument&)
            {
                PrintLine("Input must be numeric!\n\n");
            }
            catch (const std::out_of_range&)
            {
                PrintLine("Input is too large!\n\n");
            }

        } while (true);
    }

    std::string GetStringInput(const std::string& PromptText, const std::string& Default)
    {
        std::string RetVal;
        PrintLine(PromptText + ": ");
        RetVal = GetLine();
        if (RetVal.empty())
            RetVal = Default;
        return RetVal;
    }

    LinuxBaseLibrary&           m_Base;
    posix::stream_descriptor    m_Input;
    asio::streambuf             m_InputBuffer;
    posix::stream_descriptor    m_Output;

};

class ClientApp : public AppBase
{
public:
    ClientApp(LinuxBaseLibrary& BL, int clients=0, const char** addresses=NULL);

#pragma region // Himanshu
    void open_func(std::string Address);
    void associate_func();
    void polling_func(int interval=0);   // const asio::error_code& Error,
    void await_response_func(EPRI::COSEMClientEngine::RequestToken Token);
    void close_func(bool exit_on_close);
    void set_polling_interval(int seconds);
#pragma endregion // Himanshu

protected:
    void ClientMenu();
    bool IdentifyResponse(const BaseCallbackParameter& Parameters);
    bool DisconnectConfirm(const BaseCallbackParameter& Parameters);
    std::string GetInstanceIDOrDefault(ClassIDType ClassID
                , ObjectAttributeIdType AttributeID, std::string Pre = ""); // Himanshu
    void ClientMenu_Handler(const std::string& RetVal);

    LinuxClientEngine *             m_pClientEngine = nullptr;
    ISocket *                       m_pSocket = nullptr;
    ISerialSocket *                 m_pSerialSocket = nullptr;
    HDLCClientLLC *                 m_pHDLC = nullptr;
    Transport *                     m_pXPort = nullptr;
    COSEMClientEngine::RequestToken m_GetToken;
    COSEMClientEngine::RequestToken m_SetToken;
    COSEMClientEngine::RequestToken m_ActionToken;
    COSEMClientEngine::RequestToken m_AccessToken;      // Himanshu
    asio::steady_timer              m_ReadTimer;        // sudeshna
    bool m_continuous_poll = true;                      // sudeshna
    int m_waiting_interval = 10;                        // sudeshna
    // vector<std::string> TCPAddress;                  // Himanshu
    // int m_meter_idx = 0;
};

class ServerApp : public AppBase
{
public:
    // friend class COSEMServer;
    ServerApp(LinuxBaseLibrary& BL);

protected:
    void ServerMenu();
    void ServerMenu_Handler(const std::string& RetVal);

    LinuxCOSEMServerEngine * m_pServerEngine = nullptr;
    LinuxTCPSocket * m_pSocket = nullptr;              // Himanshu
};
