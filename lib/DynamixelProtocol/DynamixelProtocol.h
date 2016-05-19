#ifndef DynamixelProtocol_h
#define DynamixelProtocol_h

#define LIBRARY_VERSION 1.0.0

#include <Arduino.h>

#define DXL_NO_DATA 0xFF
#define DXL_PING 0x01
#define DXL_READ_DATA 0x02
#define DXL_WRITE_DATA 0x03
#define DXL_REG_WRITE 0x04
#define DXL_ACTION 0x05
#define DXL_RESET 0x06
#define DXL_SYNC_WRITE 0x83

class DynamixelProtocol {
  public:
    DynamixelProtocol(long baudRate, unsigned char id); //, SoftwareSerial *serial);
    void init();
    void end();
    int checkMessages();
    void sendStatusPacket(unsigned char error,
                          unsigned char *parameters, int total_parameters);
    long baudRate;
    unsigned char id;
    unsigned char instruction;
    unsigned char parameters[256];
    unsigned char total_parameters;
    unsigned char checksum;
};

#endif

