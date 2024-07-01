// g++ -o .run modbus_read.cpp -lmodbus && ./.run

#include <iostream>
#include <unistd.h>
#include <modbus/modbus.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <asm/ioctls.h>
#include <cmath>
#include <cstdio>

#include "../../config/config.h"

#define UART_PATH "/dev/ttyUSB0"
#define MODBUS_BAUD 9600

class dlms_modbus
{
    int ret;
    modbus_t *ctx;
    uint16_t *dest;
    struct timeval response_timeout;
    
public:
    dlms_modbus()
    {
        dest = new uint16_t[2];

        response_timeout.tv_sec = 10;
        response_timeout.tv_usec = 0;

        ctx = modbus_new_rtu(UART_PATH, MODBUS_BAUD, 'E', 8, 1);
        if (ctx == NULL) {
            LOG_ERROR("Unable to create the libmodbus context\n");
            ret = -1;
        }

        modbus_set_response_timeout(ctx, response_timeout.tv_sec, response_timeout.tv_usec);

        ret = modbus_set_slave(ctx, 1);
        if(ret < 0){
            LOG_ERROR("modbus_set_slave error\n");
        }
        ret = modbus_connect(ctx);
        if(ret < 0){
            LOG_ERROR("modbus_connect error\n");
        }
    }

    ~dlms_modbus()
    {
        delete []dest;
        modbus_close(ctx);
        modbus_free(ctx);
    }

    float read_register(int reg)
    {
        // Read 2 registers from adress 0, copy data to dest.
        ret = modbus_read_registers(ctx, reg, 2, dest);
        if(ret < 0){
            LOG_ERROR("modbus_read_regs error\n");
            return -1;
        }
        float f = dest_as_float();
        sleep(1);
        return f;
    }

private:
    float dest_as_float()
    {
        uint16_t b = dest[0];   //lsb
        uint16_t a = dest[1];   //msb

        float f;
        f = b/pow(2,23);
        f += (a & 127)/pow(2,7);
        f += 1 ;
        int exponent = ((a & 32640)>>7) - 127;
        f = f*pow(2,exponent);
        return f;
    }
} meter;
