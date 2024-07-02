// Himanshu

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
