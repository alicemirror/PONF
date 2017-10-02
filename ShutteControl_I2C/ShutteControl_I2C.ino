/**
 \brief Infineon TLE94112LE and XMC1100 Controlling the camera shutter + autofocus and zoom external motors 
 
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

//! Enable the shooting marker pin for timing test on different values of shooting
//! for test purpose only, disable in production.
#undef _SHOTMARK


//! I2C data string
String wData;

//! Motor control class instance
MotorControl motor;

//! If defined every command is echoed on the serial terminal
//! despite if the I2C or UART is used to send commands
#define _SERIAL_ECHO

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

  pinMode(SH_TOP, OUTPUT);
  pinMode(SH_BOTTOM, OUTPUT);
  pinMode(SHOT_MARK, OUTPUT); // for testing only

  initShutterMotor();

  // Print the initialisation message
  Serial.println(APP_TITLE);
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
 * 
 * \warning The diagnostic check based on the status of the motors running has been
 * removed from the loop as the motos control methods check by themselves the
 * diagnostic status of the TLE when a command involving a motor is executed.
 */
void loop() {

#ifdef _SERIALCONTROL
  // -------------------------------------------------------------
  // BLOCK 2 : SERIAL PARSING
  // -------------------------------------------------------------
  // Serial commands parser
  if(Serial.available() > 0){
    parseCommand(Serial.readString());
  } // serial available
#endif

} // Main loop

// ==============================================
// Shutter control functions
// ==============================================

//! Initialise the shutter motor
void initShutterMotor(void) {
  // Enable shutter motor
  motor.currentMotor = SH_MOTOR;
  motor.internalStatus[SH_MOTOR-1].isEnabled = true;
  // Set shutter motor
  motor.currentMotor = SH_MOTOR;
  // PWM Disabled
  motor.setPWM(tle94112.TLE_NOPWM);
  // Passive freewheeling
  motor.setMotorFreeWheeling(MOTOR_FW_ACTIVE);
  // Disable acceleration
  motor.setPWMRamp(RAMP_OFF);
  // Clockwise direction
  motor.setMotorDirection(MOTOR_DIRECTION_CCW);

  // Initalises the shutter windows Both solenoids released
  digitalWrite(SH_TOP, 0);
  digitalWrite(SH_BOTTOM, 0);
}

//! Exectues a single shutter motor cycle with delay
void cycleShutterMotorWithDelay(void) {
  // Start-stop 100 ms test
  motor.startMotor(SH_MOTOR);
  delay(SH_MOTOR_MS);
  motor.stopMotor(SH_MOTOR);
}

//! Lock/unlock the shutter top window
void shutterTop(boolean s) {
  if(s)
    digitalWrite(SH_TOP, 1);
  else
    digitalWrite(SH_TOP, 0);
}

//! Lock/unlock the shutter bottom window
void shutterBottom(boolean s) {
  if(s)
    digitalWrite(SH_BOTTOM, 1);
  else
    digitalWrite(SH_BOTTOM, 0);
}

//! Shooting sequence
//!
//! \param t shooting ms
void shot(int t) {
  // Lock bottom
  digitalWrite(SH_BOTTOM, 1);
  // Load load shutter
  cycleShutterMotorWithDelay();

  // Shot
  digitalWrite(SH_BOTTOM, 0);
  delay(1);
  digitalWrite(SH_TOP, 1);
  cycleShutterMotorWithDelay();
#ifdef _SHOTMARK
  digitalWrite(SHOT_MARK, 1);
#endif
  delay(t);
#ifdef _SHOTMARK
  digitalWrite(SHOT_MARK, 0);
#endif
  digitalWrite(SH_TOP, 0);
}

// ==============================================
// Message functions
// ==============================================

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
 * The command is removed from the last two characters before
 * effective parsing. Use this function when the command comes
 * from a serial terminal
 * 
 * \param commandString the string coming from the serial+CRLF
 *  ***********************************************************
 */
 void parseCommand(String cmdString) {
  int cmdlen;
  String commandString;

  commandString = cmdString;
  cmdlen = cmdString.length() - 2;
  commandString.remove(cmdlen);

  parseNoCRLF(commandString);
 }
/** ***********************************************************
 * Parse the command string and echo the executing message or 
 * command unknown error.
 * 
 * \param cmdString the string coming from the serial+CRLF
 *  ***********************************************************
 */
 void parseNoCRLF(String cmdString) {

  // =========================================================
  // Informative commands
  // =========================================================
  if(cmdString.equals(SHOW_CONF)) {
    motor.showInfo();
  }
  // =========================================================
  // Shutter motor commands
  // =========================================================
  else if(cmdString.equals(SH_MOTOR_INIT)) {
    initShutterMotor();
  }
  else if(cmdString.equals(SH_MOTOR_CYCLE)) {
    cycleShutterMotorWithDelay();
  }
  // =========================================================
  // Shutter window commands
  // =========================================================
  else if(cmdString.equals(SH_TOP_LOCK)) {
    shutterTop(true);
  }
  else if(cmdString.equals(SH_TOP_UNLOCK)) {
    shutterTop(false);
  }
  else if(cmdString.equals(SH_BOTTOM_LOCK)) {
    shutterBottom(true);
  }
  else if(cmdString.equals(SH_BOTTOM_UNLOCK)) {
    shutterBottom(false);
  }
  // =========================================================
  // Shooting commands (up to 1/1000)
  // =========================================================
  else if(cmdString.equals(SHOT_8S)) {
    shot(8000);
  }
  else if(cmdString.equals(SHOT_4S)) {
    shot(4000);
  }
  else if(cmdString.equals(SHOT_2S)) {
    shot(2000);
  }
  else if(cmdString.equals(SHOT_1S)) {
    shot(1000);
  }
  else if(cmdString.equals(SHOT_2)) {
    shot(500);
  }
  else if(cmdString.equals(SHOT_4)) {
    shot(250);
  }
  else if(cmdString.equals(SHOT_8)) {
    shot(125);
  }
  else if(cmdString.equals(SHOT_15)) {
    shot(66);
  }
  else if(cmdString.equals(SHOT_30)) {
    shot(33);
  }
  else if(cmdString.equals(SHOT_60)) {
    shot(16);
  }
  else if(cmdString.equals(SHOT_125)) {
    shot(6);
  }
  else if(cmdString.equals(SHOT_250)) {
    shot(4);
  }
  else if(cmdString.equals(SHOT_400)) {
    shot(2);
  }
  else if(cmdString.equals(SHOT_1000)) {
    shot(1);
  }
  else if(cmdString.equals(SHOT_MULTI125)) {
    int j;
    for(j = 0; j < MULTI_SHOOTING; j++)
      shot(6);
  }
  else if(cmdString.equals(SHOT_MULTI250)) {
    int j;
    for(j = 0; j < MULTI_SHOOTING; j++)
      shot(4);
  }
  else if(cmdString.equals(SHOT_MULTI400)) {
    int j;
    for(j = 0; j < MULTI_SHOOTING; j++)
      shot(2);
  }
  else if(cmdString.equals(SHOT_MULTI1000)) {
    int j;
    for(j = 0; j < MULTI_SHOOTING; j++)
      shot(1);
  }
  else
    Serial << CMD_WRONGCMD << " '" << cmdString << "'" << endl;
 }

