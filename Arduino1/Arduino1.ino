//this sketch is to communicate between 2 Mc using CAN Module MCP2515
//it will control servo motor position based on the incoming value from the other Mc
// it will send Button state to other Mc

//-----LIBRARIES------//
#include <SPI.h>
#include <mcp2515.h>
#include <Servo.h>  // include this library to control with servo

//----DEFINITIONS-----//
#define SERVO_MOTOR 9  // servo motor connected to pin D9
#define PUSH_BUTTON 5  // push button connected to pin D5
#define BUTTON_PRESSED LOW
#define BUTTON_RELEASED HIGH

//---INSTANCES----//
MCP2515 mcp2515(10);
Servo EVER_Servo;  // initialize an instance for Servo

//---------------//
struct can_frame canMsg;
int servoPosition_current = 0;
int servoPosition_previous = 0;
char buttonState = BUTTON_RELEASED;

void setup() {
  // put your setup code here, to run once:
  SPI.begin();  //Begins SPI communication
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);  //Sets CAN at speed 500KBPS and Clock 8MHz
  mcp2515.setNormalMode();
  EVER_Servo.attach(SERVO_MOTOR);      // attach servo motor to pin D9
  pinMode(PUSH_BUTTON, INPUT_PULLUP);  //connect push button with internal pull up
  pinMode(SERVO_MOTOR, OUTPUT);
  EVER_Servo.write(servoPosition_current);
}

void loop() {
  // put your main code here, to run repeatedly:

  canMsg.can_id = 0x036;  //CAN id as 0x036

  canMsg.can_dlc = 2;  //CAN data length as 2


  // get servo required position from other Mc
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    servoPosition_current = canMsg.data[1];
  }  // To receive data (Poll Read)

  // check if the servo position need to be changed and to position (0~180)
  if (servoPosition_current != servoPosition_previous) {
    if (servoPosition_current >= 0 && servoPosition_current <= 180) {
      EVER_Servo.write(servoPosition_current);
    }
  }
  servoPosition_previous = servoPosition_current;
  // toggle the led based on the button state sent from other Mc
  if (digitalRead(PUSH_BUTTON) == BUTTON_PRESSED) {
    delay(50);
    if (digitalRead(PUSH_BUTTON) == BUTTON_PRESSED) {
      buttonState = BUTTON_PRESSED;
    }
  } else {
    buttonState = BUTTON_RELEASED;
  }
  canMsg.data[0] = buttonState;  //Update buttonState value in [0]
  mcp2515.sendMessage(&canMsg);  //Sends the CAN message
  delay(100);
}
