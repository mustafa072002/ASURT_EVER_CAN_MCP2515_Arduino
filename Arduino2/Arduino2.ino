//this sketch is to communicate between 2 Mc using CAN Module MCP2515
//it will toggle led based on the button state coming from other Mc

//-----LIBRARIES------//
#include <SPI.h>
#include <mcp2515.h>
#include <Servo.h>  // include this library to control with servo

//----DEFINITIONS-----//
#define POTENTIOMETER A0  // poteniometer connected to Pin A0 on arduino
#define LED 12            // LED connected to Pin D12 on arduino
#define BUTTON_PRESSED LOW


//---INSTANCES----//
MCP2515 mcp2515(10);
//---------------//
struct can_frame canMsg;
int servoPosition_previous = 0;
int servoPosition_current = 0;
void setup() {
  // put your setup code here, to run once:
  SPI.begin();  //Begins SPI communication
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);  //Sets CAN at speed 500KBPS and Clock 8MHz
  mcp2515.setNormalMode();
  pinMode(LED, OUTPUT);
  pinMode(POTENTIOMETER, INPUT);
}

void loop() {
  canMsg.can_id = 0x037;  //CAN id as 0x037

  canMsg.can_dlc = 2;  //CAN data length as 2

  // put your main code here, to run repeatedly:
  servoPosition_current = map(analogRead(POTENTIOMETER), 0, 1023, 0, 180);  // mapping the value of poteniometer to get a position for servo (0`180)
  // check if the poteniometer value changed then send it to other Mc
  if (servoPosition_current != servoPosition_previous) {
    canMsg.data[1] = servoPosition_current; 
    mcp2515.sendMessage(&canMsg);
  }
  servoPosition_previous = servoPosition_current;
  // recieve button state from other Mc and toggle the led based on it
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    canMsg.data[0] == BUTTON_PRESSED ? digitalWrite(LED, HIGH) : digitalWrite(LED, LOW);
  }
  delay(100);
}