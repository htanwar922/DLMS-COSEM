// Himanshu

#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <io.h>
#include <ciso646>
#include <ctime>
#include <asio.hpp>

#define __attribute__(x)

typedef long long ssize_t;

// Define asio::posix::stream_descriptor using windows::stream_handle

#define STDIN_FILENO STD_INPUT_HANDLE
#define STDOUT_FILENO STD_OUTPUT_HANDLE
#define STDERR_FILENO STD_ERROR_HANDLE
#define dup(x) GetStdHandle(x)

namespace asio {
    namespace posix {
        typedef windows::stream_handle stream_descriptor;
    }
}

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
