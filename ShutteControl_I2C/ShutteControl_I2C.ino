/**
 \brief Infineon TLE94112LE and XMC1100 Controlling the camera shutter. 
 
 Needs the ShutterControl circuit to work. For more details see 
 github.com/alicemirror/PONFShutterControl
 
 I2C Version for Raspberry PI CM3: If the I2C protocol is enabled via the #define _I2CCONTROL
 Commands can be sent to the MCU via the I2C protocol
 
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics sl <www.balearicdynamics.com> SPAIN
 *  \dateSeptember 2017
 *  \version 0.1 \n
 *  Licensed under GNU LGPL 3.0
*/

#include <Wire.h>
#include <Streaming.h>
#include "commands.h"
#include "motorcontrol.h"
#include "shutter.h"

//! I2C Slave address. Set this up depending on the I2C other peripheral usage
#define SLAVE_ADDRESS 0x04

//! Define SERIALCONTROL if commands should be sent through USB to serial interface
#define _SERIALCONTROL

//! Define I2CCONTROL if commands should be sent through I2C connection
#undef _I2CCONTROL

//! I2C data string
String wData;

//! Motor control class instance
MotorControl motor;

//! Status LED (for debug purposes only)
#define LEDPIN 7

//! If defined every command is echoed on the serial terminal
//! despite if the I2C or UART is used to send commands
#define _SERIAL_ECHO

//! Running global flag
boolean isRunning;
//! Running animation frame number
int runningFrame;
//! Running frames
String runningFrames[] = { RUNNING1, RUNNING2, RUNNING3, RUNNING4 };

// ==============================================
// Initialisation
// ==============================================
void setup() {
  // Serial is initialised at high speed. If your Arduino boards
  // loose characters or show unwanted/unexpected behavior
  // try with a lower communication speed
  Serial.begin(38400);

  // initialize the motor class
  motor.begin();  

#ifdef _I2CCONTROL
  // Initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);
  // define callbacks for i2c communication
  Wire.onReceive(i2cReceiveData);
  Wire.onRequest(i2cSendData);
#endif

  pinMode(LEDPIN, OUTPUT);
  pinMode(SH_TOP, OUTPUT);
  pinMode(SH_BOTTOM, OUTPUT);

  isRunning = false;
  runningFrame = 0;

  flashLED();

  // Print the initialisation message
  Serial.println(APP_TITLE);

  initShutter();
}

// ==============================================
// Main loop
// ==============================================
/** 
 * The main loop role is execturing the service functions; display update, 
 * checking.
 * 
 * \note tHE i2c Data availability and reading from master is implemented in a
 * callback function. Data reading enable the command parsing immediately then
 * the function come back to the main loop cycle processing the other statuses
 */
void loop() {
  int j;
  boolean isRunStatus;

  // -------------------------------------------------------------
  // BLOCK 1 : MOTORS RUNNING STATUS
  // -------------------------------------------------------------
  // Check if at least one motor is running to test the error status
  // Note: It is possible to isolate the error status, if any,
  // for any motor. Here we make a simplification and check over
  // the motor without filtering
  isRunStatus = false;
  for(j = 0; j < MAX_MOTORS; j++) {
    if(motor.internalStatus[j].isRunning) {
      isRunStatus = true;
      j = MAX_MOTORS; // Force exit from loop
    } // check for running motors
  } // motors loop
  // If at least one motor is running check for diagnostic
  if(isRunStatus) {
    if(motor.tleCheckDiagnostic()) {
      motor.tleDiagnostic();
    }
  }

#ifdef _SERIALCONTROL
  // -------------------------------------------------------------
  // BLOCK 2 : SERIAL PARSING
  // -------------------------------------------------------------
  // Serial commands parser
  if(Serial.available() > 0){
    flashLED();
    parseCommand(Serial.readString());
  } // serial available
#endif

} // Main loop

// ==============================================
// Shutter control functions
// ==============================================

//! Initialise the shutter motor
void initShutter(void) {
  // Enable shutter motor
  motor.currentMotor = SH_MOTOR;
  motor.internalStatus[SH_MOTOR-1].isEnabled = true;
  // Set shutter motor
  motor.currentMotor = SH_MOTOR;
  // PWM Disabled
  motor.setPWM(tle94112.TLE_NOPWM);
  // Passive freewheeling
  motor.setMotorFreeWheeling(MOTOR_FW_PASSIVE);
  // Disable acceleration
  motor.setPWMRamp(RAMP_OFF);
  // Clockwise direction
  motor.setMotorDirection(MOTOR_DIRECTION_CW);

  // Initalises the shutter windows
  digitalWrite(SH_TOP, 0);
  digitalWrite(SH_BOTTOM, 0);
}

/** Exectues a single shutter motor cycle with delay
 */
void cycleShutterMotorWithDelay(void) {
  // Start-stop 100 ms test
  motor.startMotor(SH_MOTOR);
  delay(SH_MOTOR_MS);
  motor.stopMotors(SH_MOTOR);
}

// ==============================================
// Message functions
// ==============================================

//! Short loop flashing led for signal
void flashLED(void) {
  int j;
  for(j = 0; j < 2; j++) {
    digitalWrite(LEDPIN, 1);
    delay(50);
    digitalWrite(LEDPIN, 0);
    delay(50);
  }
}

//! Send a message to the serial
void serialMessage(String title, String description) {
#ifdef _SERIAL_ECHO
    Serial.print(title);
    Serial.print(" ");
    Serial.println(description);
#endif
}

// ==============================================
// I2C Functions
// ==============================================

/** 
 *  \brief callback for received data
 *  
 *  \param bytCount Number of bytes queued appended to command string
 */
void i2cReceiveData(int byteCount){

  wData = "";

  // Data reading
  while(Wire.available()) {
    wData += Wire.read(); // Queue 1 char to the string
  }
  parseCommand(wData);
  
}

/**
 *  \brief callback for sending data
 *  
 *  \param wData Data string to send to master
 */
void i2cSendData(){
  Wire << wData;
//  Wire.write(wData);
}

/** ***********************************************************
 * Parse the command string and echo the executing message or 
 * command unknown error.
 * 
 * \param commandString the string coming from the serial
 *  ***********************************************************
 */
 void parseCommand(String cmdString) {
  int cmdlen;
  String commandString;

  commandString = cmdString;
  cmdlen = cmdString.length() - 2;
  commandString.remove(cmdlen);

  // =========================================================
  // Informative commands
  // =========================================================
  if(commandString.equals(SHOW_CONF)) {
    motor.showInfo();
  }
  // =========================================================
  // Motor select
  // =========================================================
  else if(commandString.equals(MOTOR_1)) {
    motor.currentMotor = 1;
    serialMessage(CMD_SET, commandString);
  }
  else if(commandString.equals(MOTOR_2)) {
    motor.currentMotor = 2;
    serialMessage(CMD_SET, commandString);
  }
  else if(commandString.equals(MOTOR_3)) {
    motor.currentMotor = 3;
    serialMessage(CMD_SET, commandString);
  }
  else if(commandString.equals(MOTOR_4)) {
    motor.currentMotor = 4;
    serialMessage(CMD_SET, commandString);
  }
  else if(commandString.equals(MOTOR_5)) {
    motor.currentMotor = 5;
    serialMessage(CMD_SET, commandString);
  }
  else if(commandString.equals(MOTOR_6)) {
    motor.currentMotor = 6;
    serialMessage(CMD_SET, commandString);
  }
  // =========================================================
  // Motor enable
  // =========================================================
  else if(commandString.equals(MOTOR_ALL)) {
    int j;
    motor.currentMotor = 0;
    for(j = 0; j < MAX_MOTORS; j++) {
      motor.internalStatus[j].isEnabled = true;
    }
    serialMessage(CMD_SET, commandString);
  }
  else if(commandString.equals(MOTOR_NONE)) {
    int j;
    motor.currentMotor = 0;
    for(j = 0; j < MAX_MOTORS; j++) {
      motor.internalStatus[j].isEnabled = false;
    }
    serialMessage(CMD_SET, commandString);
  }
  else if(commandString.equals(EN_MOTOR_1)) {
    motor.currentMotor = 1;
    motor.internalStatus[0].isEnabled = true;
    serialMessage(CMD_SET, commandString);
  }
  else if(commandString.equals(EN_MOTOR_2)) {
    motor.currentMotor = 2;
    motor.internalStatus[1].isEnabled = true;
    serialMessage(CMD_SET, commandString);
  }
  else if(commandString.equals(EN_MOTOR_3)) {
    motor.currentMotor = 3;
    motor.internalStatus[2].isEnabled = true;
    serialMessage(CMD_SET, commandString);
  }
  else if(commandString.equals(EN_MOTOR_4)) {
    motor.currentMotor = 4;
    motor.internalStatus[3].isEnabled = true;
    serialMessage(CMD_SET, commandString);
  }
  else if(commandString.equals(EN_MOTOR_5)) {
    motor.currentMotor = 5;
    motor.internalStatus[4].isEnabled = true;
    serialMessage(CMD_SET, commandString);
  }
  else if(commandString.equals(EN_MOTOR_6)) {
    motor.currentMotor = 6;
    motor.internalStatus[5].isEnabled = true;
    serialMessage(CMD_SET, commandString);
  }
  // =========================================================
  // PWM channel motors assignment
  // =========================================================
  else if(commandString.equals(PWM_0)) {
    motor.setPWM(tle94112.TLE_NOPWM);
    serialMessage(CMD_PWM, commandString);
  }
  else if(commandString.equals(PWM_80)) {
    motor.setPWM(tle94112.TLE_PWM1);
    serialMessage(CMD_PWM, commandString);
  }
  else if(commandString.equals(PWM_100)) {
    motor.setPWM(tle94112.TLE_PWM2);
    serialMessage(CMD_PWM, commandString);
  }
  else if(commandString.equals(PWM_200)) {
    motor.setPWM(tle94112.TLE_PWM3);
    serialMessage(CMD_PWM, commandString);
  }
  // =========================================================
  // PWM channel select for duty cycle setting
  // =========================================================
  else if(commandString.equals(PWM80_DC)) {
    motor.currentPWM = PWM80_CHID;
    serialMessage(CMD_SET, commandString);
  }
  else if(commandString.equals(PWM100_DC)) {
    motor.currentPWM = PWM100_CHID;
    serialMessage(CMD_SET, commandString);
  }
  else if(commandString.equals(PWM200_DC)) {
    motor.currentPWM = PWM200_CHID;
    serialMessage(CMD_SET, commandString);
  }
  else if(commandString.equals(PWMALL_DC)) {
    motor.currentPWM = 0;
    serialMessage(CMD_SET, commandString);
  }
  // =========================================================
  // Direction setting
  // =========================================================
  else if(commandString.equals(DIRECTION_CW)) {
    motor.setMotorDirection(MOTOR_DIRECTION_CW);
    serialMessage(CMD_DIRECTION, commandString);
  }
  else if(commandString.equals(DIRECTION_CCW)) {
    motor.setMotorDirection(MOTOR_DIRECTION_CCW);
    serialMessage(CMD_DIRECTION, commandString);
  }
  // =========================================================
  // Freewheeling mode
  // =========================================================
  else if(commandString.equals(FW_ACTIVE)) {
    motor.setMotorFreeWheeling(MOTOR_FW_ACTIVE);
    serialMessage(CMD_MODE, commandString);
  }
  else if(commandString.equals(FW_PASSIVE)) {
    motor.setMotorFreeWheeling(MOTOR_FW_PASSIVE);
    serialMessage(CMD_MODE, commandString);
  }
  else
    Serial << CMD_WRONGCMD << " '" << commandString << "'" << endl;
 }

