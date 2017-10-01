/**
 *  \file commands.H
 *  \brief Serial commands to control the behaviour of the system.
 *  All the commands are case sensitive.
 *  
 *  Some of the original general-purpose commands has been removed as
 *  now the parsing method is finalised to the PONF-related controls.
 *  The removed generic commands are commented with the primitive calls
 *  related for further more complete documentaion.
 *  
 *  \note This version of the control is for testing only. The official firmware
 *  will use I2C controls and shorter HEX encoded commands instead.
 *  
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics sl <www.balearicdynamics.com> SPAIN
 *  \date September 2017
 *  \version 1.0
 *  Licensed under GNU LGPL 3.0
 */

#ifndef _COMMANDS
#define _COMMANDS

#undef _DEBUG_COMMANDS

// Execution notification
#define CMD_EXEC "executing "
#define CMD_DONE "done"
#define CMD_NOCMD "unknown "
#define CMD_SET "setting "
#define CMD_MODE "set "
#define CMD_DIRECTION "Direction "
#define CMD_PWM "PWM: "
#define CMD_WRONGCMD "wrong command "

// Duty cycle settings to PWM channels
#define MANUAL_DC "dcmanual"    ///< Set the duty cycle value depending on the pot
#define INFO_DC "dcinfo"        ///< Set the current duty cycle values

// Configuration command
#define SHOW_CONF "conf"    ///< Dump the current settings

// Shutter commands
#define CMD_SH_MOTOR_CYCLE

/* ***********************************************************
#define MOTOR_START "start"   ///< start all
#define MOTOR_STOP "stop"     ///< stop all
#define MOTOR_RESET "reset"   ///< reset the system to the default

// =========================================================
// Motor actions
// =========================================================
if(commandString.equals(MOTOR_RESET)) {
  Serial << CMD_EXEC << " '" << commandString << "'" << endl;
  motor.reset();
  Serial << CMD_DONE << endl;
}
if(commandString.equals(MOTOR_START)) {
  motor.startMotors();
  isRunning = true;
}
if(commandString.equals(MOTOR_STOP)) {
  motor.stopMotors();
  isRunning = false;
}

#define PWM_RAMP "accel"        ///< Enable the acceleration
#define PWM_NORAMP "noaccel"    ///< Disable the acceleration

// =========================================================
// PWM ramp settings
// =========================================================
if(commandString.equals(PWM_RAMP)) {
  motor.setPWMRamp(RAMP_ON);
  serialMessage(CMD_MODE, commandString);
}
if(commandString.equals(PWM_NORAMP)) {
  motor.setPWMRamp(RAMP_OFF);
  serialMessage(CMD_MODE, commandString);
}

#define MOTOR_1 "m1"        ///< select motor
#define MOTOR_2 "m2"        ///< select motor
#define MOTOR_3 "m3"        ///< select motor
#define MOTOR_4 "m4"        ///< select motor
#define MOTOR_5 "m5"        ///< select motor
#define MOTOR_6 "m6"        ///< select motor

// =========================================================
// Motor select
// =========================================================
if(commandString.equals(MOTOR_1)) {
  motor.currentMotor = 1;
  serialMessage(CMD_SET, commandString);
}
if(commandString.equals(MOTOR_2)) {
  motor.currentMotor = 2;
  serialMessage(CMD_SET, commandString);
}
if(commandString.equals(MOTOR_3)) {
  motor.currentMotor = 3;
  serialMessage(CMD_SET, commandString);
}
if(commandString.equals(MOTOR_4)) {
  motor.currentMotor = 4;
  serialMessage(CMD_SET, commandString);
}
if(commandString.equals(MOTOR_5)) {
  motor.currentMotor = 5;
  serialMessage(CMD_SET, commandString);
}
if(commandString.equals(MOTOR_6)) {
  motor.currentMotor = 6;
  serialMessage(CMD_SET, commandString);
}

#define MOTOR_ALL "all"     ///< All motors selected and enabled
#define MOTOR_NONE "none"   ///< No motors disabled and disabled
#define EN_MOTOR_1 "m1+"    ///< enable motor
#define EN_MOTOR_2 "m2+"    ///< enable motor
#define EN_MOTOR_3 "m3+"    ///< enable motor
#define EN_MOTOR_4 "m4+"    ///< enable motor
#define EN_MOTOR_5 "m5+"    ///< enable motor
#define EN_MOTOR_6 "m6+"    ///< enable motor

// =========================================================
// Motor enable
// =========================================================
if(commandString.equals(MOTOR_ALL)) {
  int j;
  motor.currentMotor = 0;
  for(j = 0; j < MAX_MOTORS; j++) {
    motor.internalStatus[j].isEnabled = true;
  }
  serialMessage(CMD_SET, commandString);
}
if(commandString.equals(MOTOR_NONE)) {
  int j;
  motor.currentMotor = 0;
  for(j = 0; j < MAX_MOTORS; j++) {
    motor.internalStatus[j].isEnabled = false;
  }
  serialMessage(CMD_SET, commandString);
}
if(commandString.equals(EN_MOTOR_1)) {
  motor.currentMotor = 1;
  motor.internalStatus[0].isEnabled = true;
  serialMessage(CMD_SET, commandString);
}
if(commandString.equals(EN_MOTOR_2)) {
  motor.currentMotor = 2;
  motor.internalStatus[1].isEnabled = true;
  serialMessage(CMD_SET, commandString);
}
if(commandString.equals(EN_MOTOR_3)) {
  motor.currentMotor = 3;
  motor.internalStatus[2].isEnabled = true;
  serialMessage(CMD_SET, commandString);
}
if(commandString.equals(EN_MOTOR_4)) {
  motor.currentMotor = 4;
  motor.internalStatus[3].isEnabled = true;
  serialMessage(CMD_SET, commandString);
}
if(commandString.equals(EN_MOTOR_5)) {
  motor.currentMotor = 5;
  motor.internalStatus[4].isEnabled = true;
  serialMessage(CMD_SET, commandString);
}
if(commandString.equals(EN_MOTOR_6)) {
  motor.currentMotor = 6;
  motor.internalStatus[5].isEnabled = true;
  serialMessage(CMD_SET, commandString);
}

#define PWM_0    "noPWM"  ///< No PWM
#define PWM_80   "80"     ///< PWM 80 Hz
#define PWM_100  "100"    ///<PWM 100 Hz
#define PWM_200  "200"    ///< PWM 200 Hz

// =========================================================
// PWM channel motors assignment
// =========================================================
if(commandString.equals(PWM_0)) {
  motor.setPWM(tle94112.TLE_NOPWM);
  serialMessage(CMD_PWM, commandString);
}
if(commandString.equals(PWM_80)) {
  motor.setPWM(tle94112.TLE_PWM1);
  serialMessage(CMD_PWM, commandString);
}
if(commandString.equals(PWM_100)) {
  motor.setPWM(tle94112.TLE_PWM2);
  serialMessage(CMD_PWM, commandString);
}
if(commandString.equals(PWM_200)) {
  motor.setPWM(tle94112.TLE_PWM3);
  serialMessage(CMD_PWM, commandString);
}

#define PWM80_DC "dc80"         ///< Set the duty cycle to the PWM channel 80Hz
#define PWM100_DC "dc100"       ///< Set the duty cycle to the PWM channel 100Hz
#define PWM200_DC "dc200"       ///< Set the duty cycle to the PWM channel 200Hz
#define PWMALL_DC "dcPWM"       ///< Set the duty cycle to the all the PWM channels

// =========================================================
// PWM channel select for duty cycle setting
// =========================================================
if(commandString.equals(PWM80_DC)) {
  motor.currentPWM = PWM80_CHID;
  serialMessage(CMD_SET, commandString);
}
if(commandString.equals(PWM100_DC)) {
  motor.currentPWM = PWM100_CHID;
  serialMessage(CMD_SET, commandString);
}
if(commandString.equals(PWM200_DC)) {
  motor.currentPWM = PWM200_CHID;
  serialMessage(CMD_SET, commandString);
}
if(commandString.equals(PWMALL_DC)) {
  motor.currentPWM = 0;
  serialMessage(CMD_SET, commandString);
}

#define DIRECTION_CW "cw"     ///< clockwise rotation
#define DIRECTION_CCW "ccw"   ///< counterclockwise rotation

// =========================================================
// Direction setting
// =========================================================
if(commandString.equals(DIRECTION_CW)) {
  motor.setMotorDirection(MOTOR_DIRECTION_CW);
  serialMessage(CMD_DIRECTION, commandString);
}
if(commandString.equals(DIRECTION_CCW)) {
  motor.setMotorDirection(MOTOR_DIRECTION_CCW);
  serialMessage(CMD_DIRECTION, commandString);
}

#define FW_ACTIVE "fwactive"    ///< Fereewheeling active
#define FW_PASSIVE "fwpassive"  ///< Freewheeling passive

// =========================================================
// Freewheeling mode
// =========================================================
if(commandString.equals(FW_ACTIVE)) {
  motor.setMotorFreeWheeling(MOTOR_FW_ACTIVE);
  serialMessage(CMD_MODE, commandString);
}
if(commandString.equals(FW_PASSIVE)) {
  motor.setMotorFreeWheeling(MOTOR_FW_PASSIVE);
  serialMessage(CMD_MODE, commandString);
}
************************************************************** */

#endif
