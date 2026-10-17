#ifndef MSL_C_UART_H
#define MSL_C_UART_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int UARTError;

enum {
    kUARTNoError = 0,
    kUARTUnknownBaudRate,
    kUARTConfigurationError,
    kUARTBufferOverflow,
    kUARTNoData
};

typedef enum {
    kBaudHWSet = -1,
    kBaud300 = 300,
    kBaud600 = 600,
    kBaud1200 = 1200,
    kBaud1800 = 1800,
    kBaud2000 = 2000,
    kBaud2400 = 2400,
    kBaud3600 = 3600,
    kBaud4800 = 4800,
    kBaud7200 = 7200,
    kBaud9600 = 9600,
    kBaud19200 = 19200,
    kBaud38400 = 38400,
    kBaud57600 = 57600,
    kBaud115200 = 115200,
    kBaud230400 = 230400
} UARTBaudRate;

UARTError InitializeUART(UARTBaudRate baudRate);
UARTError WriteUARTN(const void* bytes, unsigned long length);

#ifdef __cplusplus
}
#endif

#endif
