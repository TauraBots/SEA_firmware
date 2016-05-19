#include "DynamixelProtocol.h"

DynamixelProtocol::DynamixelProtocol(long baudRate, unsigned char id) //, SoftwareSerial *serial)
{
  this->baudRate = baudRate;
  this->id = id;
  this->instruction = DXL_NO_DATA;
  this->total_parameters = 0;
}

void DynamixelProtocol::init()
{
  Serial.begin(baudRate);
  Serial.setTimeout(50);
}

void DynamixelProtocol::end()
{
  Serial.end();
}

int DynamixelProtocol::checkMessages()
{
  int c;
  int state = 0;
  int length = 2;
  //unsigned char checksum;
  unsigned char sum_params = 0;
  unsigned char dst_id = 0;
  int quit = 0;
  this->instruction = DXL_NO_DATA;
  while (!quit)
  {
    // Serial availability check
    //if (Serial.available() > 0) {
      c = Serial.read();
      if (c != -1) // && c != 0)
      {
        switch (state)
        {
          case 0:
          case 1:
            if ((unsigned char)c == 0xFF) {
              state = state + 1;
            } else {
              state = 0;
              this->total_parameters = 0;
              quit = 1;
            }
            break;
          case 2:
            if ((unsigned char)c == 0xFF) break;
            dst_id = (unsigned char)c;
            instruction = DXL_NO_DATA;
            sum_params = 0;
            if (dst_id == this->id) {
              state = 3;
            } else {
              //state = 10;
              state = 0;
              this->total_parameters = 0;
              quit = 1;
            }
            break;
          case 3:
          case 10:
            length = (unsigned char)c;
            this->total_parameters = length - 2;
            if (length < 2) {
              state = 0;
              this->total_parameters = 0;
              quit = 1;
              break;
            }
            state = state + 1;
            break;
          case 4:
          case 11:
            length = length - 1;
            if (state == 4) instruction = (unsigned char)c;
            sum_params = sum_params + c;
            state = state + 1;
            break;
          case 5:
          case 12:
            length = length - 1;
            if (length > 0)
            {
              if (state == 5) parameters[this->total_parameters - length] = (unsigned char)c;
              sum_params = sum_params + (unsigned char)c;
            } else {
              this->checksum = (unsigned char)c;
              unsigned char test = ~(dst_id + (this->total_parameters + 2) + sum_params);
              if (this->checksum != test)
              {
                instruction = DXL_NO_DATA;
                total_parameters = 0;
              }
              state = 0;
              quit = 1;
            }
            break;
        }
      }
    //} else { // Serial availability check
    //  state = 0;
    //  total_parameters = 0;
    //  quit = 1;
    //}
  }
}

void DynamixelProtocol::sendStatusPacket(unsigned char error,
                                    unsigned char *parameters, int total_parameters)
{
  unsigned char length = total_parameters + 2;
  unsigned char checksum = this->id + length + error;
  for (int i = 0 ; i < total_parameters ; i++)
    checksum += parameters[i];
  checksum = ~checksum;
  unsigned char message[256];
  for (int i = 0 ; i < 256 ; i++) message[i] = 0;
  message[0] = 0xff;
  message[1] = 0xff;
  message[2] = this->id;
  message[3] = length;
  message[4] = error;
  for (int i = 0 ; i < total_parameters ; i++)
  {
    message[5+i] = parameters[i];
  }
  message[5+total_parameters] = checksum;
  Serial.write((uint8_t *)message,total_parameters+6);
}
