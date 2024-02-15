#include <mcp2515.h> (https://github.com/autowp/arduino-mcp2515/)
#define MCP_CS 10

MCP2515 mcp2515(MCP_CS);
struct can_frame canMsg;

void setup() {
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ); 
  mcp2515.setNormalMode();
}

void setRPM() {
    unsigned int rpm = ((unsigned int) canMsg.data[3] << 8) | canMsg.data[2];
    rpm = rpm * 4; //Hyundai Elantra RPM gauge has a 0.25 multiplier so we need to multiply the RPM from the motor 4 times.

    canMsg.can_id  = 0x316; //Hyundai Elantra uses CANBUS ID 0x316 for RPM message
    canMsg.can_dlc = 8;
    canMsg.data[0] = 0x00;
    canMsg.data[1] = 0x00;
    if (rpm <= 2048) {
      canMsg.data[3] = 0x08; //send at least 512 RPM for the EPS to activate and remain active
      canMsg.data[2] = 0x00;
    } else {
      canMsg.data[2] = (unsigned char) rpm;
      canMsg.data[3] = (unsigned char) (rpm >> 8);
    }
    canMsg.data[4] = 0x00;
    canMsg.data[5] = 0x00;
    canMsg.data[6] = 0x00;
    canMsg.data[7] = 0x00;
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK && canMsg.can_id == 0x317) // To receive RPM from motor controller using CANBUS ID 0x317 (Poll Read)
  {
    setRPM();
    mcp2515.sendMessage(&canMsg);
    delay(1);
  }
}