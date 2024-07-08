// Himanshu

#ifdef _MSC_VER

#include "msvc-config.h"

LARGE_INTEGER getFILETIMEoffset()
{
    SYSTEMTIME s;
    FILETIME f;
    LARGE_INTEGER t;

    s.wYear = 1970;
    s.wMonth = 1;
    s.wDay = 1;
    s.wHour = 0;
    s.wMinute = 0;
    s.wSecond = 0;
    s.wMilliseconds = 0;
    SystemTimeToFileTime(&s, &f);
    t.QuadPart = f.dwHighDateTime;
    t.QuadPart <<= 32;
    t.QuadPart |= f.dwLowDateTime;
    return (t);
}

int clock_gettime(int X, struct timespec* tv)
{
    LARGE_INTEGER           t;
    FILETIME            f;
    double                  microseconds;
    static LARGE_INTEGER    offset;
    static double           frequencyToMicroseconds;
    static int              initialized = 0;
    static BOOL             usePerformanceCounter = 0;

    if (!initialized) {
        LARGE_INTEGER performanceFrequency;
        initialized = 1;
        usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
        if (usePerformanceCounter) {
            QueryPerformanceCounter(&offset);
            frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
        }
        else {
            offset = getFILETIMEoffset();
            frequencyToMicroseconds = 10.;
        }
    }
    if (usePerformanceCounter) QueryPerformanceCounter(&t);
    else {
        GetSystemTimeAsFileTime(&f);
        t.QuadPart = f.dwHighDateTime;
        t.QuadPart <<= 32;
        t.QuadPart |= f.dwLowDateTime;
    }

    t.QuadPart -= offset.QuadPart;
    microseconds = (double)t.QuadPart / frequencyToMicroseconds;
    t.QuadPart = microseconds;
    tv->tv_sec = t.QuadPart / 1000000;
    tv->tv_nsec = t.QuadPart % 1000000 * 1000;
    return (0);
}

int usleep(unsigned int usec)
{
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -(10 * (__int64)usec);

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);

    return 0;
}


int tcflush(HANDLE h, int queue)
{
    DWORD flags = 0;
    switch (queue) {
    case TCIFLUSH:
        flags = PURGE_RXABORT | PURGE_RXCLEAR;
        break;
    case TCIOFLUSH:
        flags = PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR;
        break;
    case TCOFLUSH:
        flags = PURGE_TXABORT | PURGE_TXCLEAR;
        break;
    }
    return PurgeComm(h, flags) == 0;
}

int sem_init(sem_t* sem, int pshared, unsigned int value)
{
    *sem = CreateSemaphore(NULL, value, 1, NULL);
    return *sem == NULL;
}

int sem_destroy(sem_t* sem)
{
    return CloseHandle(*sem) == 0;
}

int sem_wait(sem_t* sem)
{
    return WaitForSingleObject(*sem, INFINITE) == WAIT_FAILED;
}

int sem_post(sem_t* sem)
{
    return ReleaseSemaphore(*sem, 1, NULL) == 0;
}

int GetHandle(int n)
{
    return n;
}

void asio::posix::tcp_echo::StreamHandleTCPEchoClient::connect(const std::string& host, const std::string& port) {
    tcp::resolver resolver(get_executor());
    auto endpoints = resolver.resolve(host, port);

    asio::async_connect(*this, endpoints,
        [this](asio::error_code ec, tcp::endpoint) {
            if (!ec) {
                std::cout << "Connected to server!" << std::endl;
                read();
            }
            else {
                std::cerr << "Connect failed: " << ec.message() << std::endl;
            }
        }
    );
}

void asio::posix::tcp_echo::StreamHandleTCPEchoClient::read() {
    asio::async_read_until(*this, buffer_, '\n',
        [this](asio::error_code ec, std::size_t length) {
            if (!ec) {
                std::istream is(&buffer_);
                std::string line;
                std::getline(is, line);
                std::cout << line << std::endl;

                // Continue reading
                read();
            }
            else {
                std::cerr << "Read failed: " << ec.message() << std::endl;
                close();
            }
        }
    );
}

void asio::posix::tcp_echo::StreamHandleTCPEchoClient::write(const std::string& data) {
    std::cout << data;
    asio::async_write(*this, asio::buffer(data),
        [this](asio::error_code ec, std::size_t length) {
            if (!ec) {
                std::cout << "Sent " << length << " bytes." << std::endl;
            }
            else {
                std::cerr << "Write failed: " << ec.message() << std::endl;
            }
        }
    );
}

void asio::posix::tcp_echo::StreamHandleTCPEchoClient::launch_detached_process(std::string cmd) {
    STARTUPINFO si;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;  // Ensure the window is hidden
    si.wShowWindow = SW_HIDE;  // Hide the window

    ZeroMemory(&pi_, sizeof(pi_));

    // Create the process
    if (!CreateProcess(NULL,   // No module name (use command line)
        (LPSTR)cmd.c_str(),    // Command line
        NULL,                  // Process handle not inheritable
        NULL,                  // Thread handle not inheritable
        FALSE,                 // Set handle inheritance to FALSE
        CREATE_NEW_CONSOLE,    // Create a new console window for the process
        NULL,                  // Use parent's environment block
        NULL,                  // Use parent's starting directory 
        &si,                   // Pointer to STARTUPINFO structure
        &pi_                   // Pointer to PROCESS_INFORMATION structure
    ))
    {
        std::cerr << "CreateProcess failed" << std::endl;
        return;
    }
}

void asio::posix::tcp_echo::StreamHandleTCPEchoClient::close_detached_process() {
    // Close process and thread handles (we don't need to wait for it)
    CloseHandle(pi_.hProcess);
    CloseHandle(pi_.hThread);
}

#endif // _MSC_VER