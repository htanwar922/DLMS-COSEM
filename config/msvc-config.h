// Himanshu

#pragma once

#define _WIN32_WINNT _WIN32_WINNT_WIN10

#include <WinSock2.h>
#include <Windows.h>
#include <io.h>
#include <ciso646>
#include <ctime>
#include <asio.hpp>

#include <iostream>
#include <string>

#define __attribute__(x)

typedef long long ssize_t;

// Define the functionality of clock_gettime using Windows API

#define CLOCK_MONOTONIC 1

LARGE_INTEGER getFILETIMEoffset();
int clock_gettime(int X, struct timespec* tv);

// Define the functionality of usleep using Windows API

int usleep(unsigned int usec);

// Define the functionality of tcflush using Windows API

#define TCIFLUSH 0
#define TCIOFLUSH 1
#define TCOFLUSH 2

int tcflush(HANDLE h, int queue);

// Define the functionality of semaphore.h using Windows API

typedef HANDLE sem_t;

int sem_init(sem_t* sem, int pshared, unsigned int value);
int sem_destroy(sem_t* sem);
int sem_wait(sem_t* sem);
int sem_post(sem_t* sem);

// Define asio::posix::stream_descriptor using asio::ip::tcp::socket

#define STDIN_FILENO 8080
#define STDOUT_FILENO 8081
#define STDERR_FILENO 8082

using namespace asio;
using asio::ip::tcp;

int GetHandle(int n);
#define dup(x) GetHandle(x)
//#define dup(x) *asio::ip::tcp::resolver(m_IO).resolve("localhost", std::to_string(x)).begin()

namespace asio {
    namespace posix {
        namespace tcp_echo {
            class StreamHandleTCPEchoClient : public asio::basic_stream_socket<tcp> {
                // Members for io_context and TCP socket
                asio::streambuf buffer_;
                int port_;
                PROCESS_INFORMATION pi_;

            public:
                // Constructor to initialize the io_context and socket
                StreamHandleTCPEchoClient(asio::io_context& io_context, int port)
                : asio::basic_stream_socket<tcp>(io_context)
                , port_(port)
                {
                    launch_detached_process("python '" ROOT_DIR "/tcp_echo_server.py' " + std::to_string(port_));
                    connect("localhost", std::to_string(port_));
                }

                ~StreamHandleTCPEchoClient() {
                    close_detached_process();
                }

                // Resolve endpoint and connect to the server
                void connect(const std::string& host, const std::string& port);

                // Async write data to the socket
                void write(const std::string& data);

            private:

                // Async read data from the socket
                void read();

                // Launch a detached process
                void launch_detached_process(std::string cmd);
                void close_detached_process();
            };
        }
        typedef tcp_echo::StreamHandleTCPEchoClient stream_descriptor;
        //typedef asio::basic_stream_socket<tcp> stream_descriptor;
    }
}

// Undefine the conflicting macros defined in Windows.h

#undef min
#undef max

#undef ABSOLUTE
#undef RELATIVE

#undef BAUD_300
#undef BAUD_600
#undef BAUD_1200
#undef BAUD_1800
#undef BAUD_2400
#undef BAUD_4800
#undef BAUD_9600
#undef BAUD_19200
#undef BAUD_38400
#undef BAUD_57600
#undef BAUD_115200

#undef PARITY_NONE
#undef PARITY_EVEN
#undef PARITY_ODD

#undef VOID
#undef OPTIONAL

#undef ST_CONNECTED
