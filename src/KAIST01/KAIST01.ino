#include <Arduino.h>
#include <DynamixelProtocol.h>
#include <AS5048A.h>
#include <SPI.h>
#include <SoftwareSerial.h>

#define SEA_ID 108
#define LED 8
#define EN 2
#define INVERTED

AS5048A mag(10);
//SoftwareSerial Serial(0,1);
DynamixelProtocol dxl(400000,SEA_ID); //,&Serial);

void blink(int times)
{
  for (int i = 0 ; i < times ; i++)
  {
    digitalWrite(LED,HIGH);
    delay(125);
    digitalWrite(LED,LOW);
    delay(125);
  }
  delay(500);
}

void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(EN, OUTPUT);
  digitalWrite(EN, LOW);
  mag.init();
  dxl.init();
  blink(SEA_ID - 100);
}

void loop() {
  #ifndef INVERTED
  word val = mag.getRawRotation();
  #else
  word val = 8192 - mag.getRawRotation();
  #endif
  dxl.checkMessages();
  if (dxl.instruction != DXL_NO_DATA)
  {
    digitalWrite(LED, HIGH);
    switch (dxl.instruction)
    {
      case DXL_PING:
        digitalWrite(EN, HIGH);
        dxl.sendStatusPacket(0x00, NULL, 0);
        Serial.flush();
        digitalWrite(EN, LOW);
        break;
      case DXL_READ_DATA:
        if (dxl.total_parameters == 2 && dxl.parameters[1] == 2)
        {
          unsigned char values[2];
          switch (dxl.parameters[0])
          {
            case 0x24:
              values[0] = (unsigned char)val;
              values[1] = (unsigned char)(val >> 8);
              break;
            case 0x00:
              values[0] = 54;    
              values[1] = 1;
              break;
            case 0x01:
              values[0] = 0;
              values[1] = 1;
              break;
            default:
              values[0] = 0;
              values[1] = 0;
              break;
          }
          //delay(1);
          digitalWrite(EN, HIGH);
          dxl.sendStatusPacket(0x00, values, 2);
          Serial.flush();
          digitalWrite(EN, LOW);
        }
        break;
      case DXL_WRITE_DATA:
        break;
      case DXL_REG_WRITE:
        break;
      case DXL_ACTION:
        break;
      case DXL_RESET:
        break;
      case DXL_SYNC_WRITE:
        break;
      default:
        break; 
    }
  } else {
    digitalWrite(LED,LOW);
  }
}

