/** 
 *  \file motorcontrol.cpp
 *  \brief This file defines functions and predefined instances from motorcontrol.h
 *  
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics sl <www.balearicdynamics.com> SPAIN
 *  \date July 2017, update September 2017
 *  \version 2.0
 *  Licensed under GNU LGPL 3.0
 */

#include "motorcontrol.h"

// ===============================================================
// Initialization and reset methods
// ===============================================================

void MotorControl::begin(void) {
  // enable tle94112
  tle94112.begin();
  
  reset();
}

void MotorControl::end(void) {
  tle94112.end();
}

void MotorControl::reset() {
  int j;

  for(j = 0; j < MAX_MOTORS; j++) {
    internalStatus[j].channelPWM = tle94112.TLE_NOPWM; // PWM disabled on start
    internalStatus[j].isEnabled = false;    // Motors initially disabled
    internalStatus[j].isRunning = false;    // Not running (should be enabled)
    internalStatus[j].freeWheeling = true;  // Free wheeling active
    internalStatus[j].motorDirection = MOTOR_DIRECTION_CW;
  } // loop on the motors array

  for(j = 0; j < AVAIL_PWM_CHANNELS; j++) {
    dutyCyclePWM[j].minDC = DUTYCYCLE_MIN;  // Min duty cycle
    dutyCyclePWM[j].maxDC = DUTYCYCLE_MAX;  // Max duty cycle
    dutyCyclePWM[j].manDC = false;          // Duty cycle in auto mode
    dutyCyclePWM[j].useRamp = false;        // No acceleration
  } // loop on the PWM channels array

  resetHB();
  resetPWM();
  currentPWM = 0; // No PWM channels selected
  currentMotor = 0; // No motors selected
}

void MotorControl::resetHB(void) {
  // Set all the half bridges floating without pwm
  tle94112.configHB(tle94112.TLE_HB1, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB3, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB4, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB5, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB6, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB7, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB8, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB9, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB10, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB11, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB12, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
}

void MotorControl::resetPWM(void) {
  // Initialize the PWM channels to the corresponding frequency and duty cycle 0
  tle94112.configPWM(tle94112.TLE_PWM1, tle94112.TLE_FREQ80HZ, (uint8_t)0);
  tle94112.configPWM(tle94112.TLE_PWM2, tle94112.TLE_FREQ100HZ, (uint8_t)0);
  tle94112.configPWM(tle94112.TLE_PWM3, tle94112.TLE_FREQ200HZ, (uint8_t)0);
}

// ===============================================================
// Setting motors configuration
// ===============================================================

void MotorControl::setPWM(uint8_t pwmCh) {
  if(currentMotor != 0) {
    internalStatus[currentMotor - 1].channelPWM = pwmCh;
  }
  else {
    int j;
    for (j = 0; j < MAX_MOTORS; j++) {
      internalStatus[j].channelPWM = pwmCh;
    }
  }
}

void MotorControl::setMotorDirection(int dir) {
  if(currentMotor != 0) {
    internalStatus[currentMotor - 1].motorDirection = dir;
  }
  else {
    int j;
    for (j = 0; j < MAX_MOTORS; j++) {
      internalStatus[j].motorDirection = dir;
    }
  }
}

void MotorControl::setMotorFreeWheeling(boolean fw) {
  if(currentMotor != 0) {
    internalStatus[currentMotor - 1].freeWheeling = fw;
  }
  else {
    int j;
    for (j = 0; j < MAX_MOTORS; j++) {
      internalStatus[j].freeWheeling = fw;
    }
  }
}

// ===============================================================
// Setting PWM methods
// ===============================================================

void MotorControl::setPWMManualDC(boolean dc) {
  if(currentPWM != 0) {
    dutyCyclePWM[currentPWM - 1].manDC = dc;
  }
  else {
    int j;
    for (j = 0; j < AVAIL_PWM_CHANNELS; j++) {
      dutyCyclePWM[j].manDC = dc;
    }
  }
}

void MotorControl::setPWMMinDC(uint8_t dc) {
  if(currentPWM != 0) {
    // We are setting a specific PWM channel
    dutyCyclePWM[currentPWM - 1].minDC = dc;
  }
  else {
    int j;
    for (j = 0; j < AVAIL_PWM_CHANNELS; j++) {
      dutyCyclePWM[j].minDC = dc;
    }
  }
}

void MotorControl::setPWMMaxDC(uint8_t dc) {
  if(currentPWM != 0) {
    // We are setting a specific PWM channel
    dutyCyclePWM[currentPWM - 1].maxDC = dc;
  }
  else {
    int j;
    for (j = 0; j < AVAIL_PWM_CHANNELS; j++) {
      dutyCyclePWM[j].maxDC = dc;
    }
  }
}

void MotorControl::setPWMRamp(boolean acc) {
  if(currentPWM != 0) {
    dutyCyclePWM[currentPWM - 1].useRamp = acc;
  }
  else {
    int j;
    for (j = 0; j < AVAIL_PWM_CHANNELS; j++) {
      dutyCyclePWM[j].useRamp = acc;
    }
  }
}

// ===============================================================
// Motor control action
// ===============================================================

void MotorControl::startMotors(void) {
  motorConfigHB();
  motorPWMStart();
}

void MotorControl::stopMotors(void) {
  motorPWMStop();
  motorStopHB();
}

void MotorControl::startMotor(int m) {
  motorConfigHB(m - 1);
  motorPWMStart();
}

void MotorControl::stopMotor(int m) {
  motorPWMStop();
  motorStopHB(m - 1);
  if(tleCheckDiagnostic())
    tleDiagnostic(j, TLE_MOTOR_STOPPING);
}

void MotorControl::motorPWMAnalogDC(void) {
  int j;
  
  // Loop on the PWM channels
  for (j = 0; j < AVAIL_PWM_CHANNELS; j++) {
    // See if the channel is set for manual dutycycle
    if(dutyCyclePWM[j].manDC) {
      // If new value is bigger than previous, arrange
      // the DC value for acceleration else do the opposite
      if(lastAnalogDC > prevAnalogDC) {
        dutyCyclePWM[j].minDC = prevAnalogDC;
        dutyCyclePWM[j].maxDC = lastAnalogDC;
        motorPWMAccelerate(j);
      }
      else {
        dutyCyclePWM[j].maxDC = prevAnalogDC;
        dutyCyclePWM[j].minDC = lastAnalogDC;
        motorPWMDecelerate(j);
      }
    }
  }
}

void MotorControl::motorPWMStart(void) {
  int j;

  hasManualDC = false;
  
  // Loop on the PWM channels
  for (j = 0; j < AVAIL_PWM_CHANNELS; j++) {
    // See if the channel is set for manual dutycycle
    if(dutyCyclePWM[j].manDC)
      hasManualDC = true; // Save the global flag for the program logic
    // Start PWM channel of acceleration cycle
    if(dutyCyclePWM[j].useRamp) {
      // Should manage acceleration
      motorPWMAccelerate(j);
    }
    else
      motorPWMRun(j);
  }
}

void MotorControl::motorPWMStop(void) {
  int j;
  
  // Loop on the PWM channels
  for (j = 0; j < AVAIL_PWM_CHANNELS; j++) {
    if(dutyCyclePWM[j].useRamp) {
      // Should manage acceleration
      motorPWMDecelerate(j);
      motorPWMHalt(j);
    }
    else
      motorPWMHalt(j);
  }
}

void MotorControl::motorPWMAccelerate(int channel) {
  int j;

  for(j = dutyCyclePWM[channel].minDC; j < dutyCyclePWM[channel].maxDC; j++) {
    switch(channel + 1){
      case PWM80_CHID:
        tle94112.configPWM(tle94112.TLE_PWM1, tle94112.TLE_FREQ80HZ, (uint8_t)j);
      break;
      case PWM100_CHID:
        tle94112.configPWM(tle94112.TLE_PWM2, tle94112.TLE_FREQ100HZ, (uint8_t)j);
      break;
      case PWM200_CHID:
        tle94112.configPWM(tle94112.TLE_PWM3, tle94112.TLE_FREQ200HZ, (uint8_t)j);
      break;
    }
    //Check for error
    if(tleCheckDiagnostic()) {
      tleDiagnostic();
    }
    delay(RAMP_STEP_DELAY);
  }
}

void MotorControl::motorPWMRun(int channel) {
  switch(channel + 1){
    case PWM80_CHID:
      tle94112.configPWM(tle94112.TLE_PWM1, tle94112.TLE_FREQ80HZ, dutyCyclePWM[channel].maxDC);
    break;
    case PWM100_CHID:
      tle94112.configPWM(tle94112.TLE_PWM2, tle94112.TLE_FREQ100HZ, dutyCyclePWM[channel].maxDC);
    break;
    case PWM200_CHID:
      tle94112.configPWM(tle94112.TLE_PWM3, tle94112.TLE_FREQ200HZ, dutyCyclePWM[channel].maxDC);
    break;
  }
}

void MotorControl::motorPWMHalt(int channel) {
  switch(channel + 1){
    case PWM80_CHID:
      tle94112.configPWM(tle94112.TLE_PWM1, tle94112.TLE_FREQ80HZ, (uint8_t)0);
    break;
    case PWM100_CHID:
      tle94112.configPWM(tle94112.TLE_PWM2, tle94112.TLE_FREQ100HZ, (uint8_t)0);
    break;
    case PWM200_CHID:
      tle94112.configPWM(tle94112.TLE_PWM3, tle94112.TLE_FREQ200HZ, (uint8_t)0);
    break;
  }
}

void MotorControl::motorPWMDecelerate(int channel) {
  int j;
  
  for(j = dutyCyclePWM[channel].maxDC; j > dutyCyclePWM[channel].minDC; j--) {
    // Update the speed
    switch(channel + 1){
      case PWM80_CHID:
        tle94112.configPWM(tle94112.TLE_PWM1, tle94112.TLE_FREQ80HZ, (uint8_t)j);
      break;
      case PWM100_CHID:
        tle94112.configPWM(tle94112.TLE_PWM2, tle94112.TLE_FREQ100HZ, (uint8_t)j);
      break;
      case PWM200_CHID:
        tle94112.configPWM(tle94112.TLE_PWM3, tle94112.TLE_FREQ200HZ, (uint8_t)j);
      break;
    }
    //Check for error
    if(tleCheckDiagnostic()) {
      tleDiagnostic();
    }
    delay(RAMP_STEP_DELAY);
  }
}

// ===============================================================
// Half bridges configuraton
// ===============================================================

void MotorControl::motorConfigHB(void) {
  int j;
    for(j = 0; j < MAX_MOTORS; j++) {
      motorConfigHB(j);
    }
}

void MotorControl::motorConfigHB(int motor) {
  if(internalStatus[motor].isEnabled) {
    if(internalStatus[motor].motorDirection == MOTOR_DIRECTION_CW)
      motorConfigHBCW(motor);
    else
      motorConfigHBCCW(motor);

    if(tleCheckDiagnostic())
      tleDiagnostic(motor, TLE_MOTOR_STARTING);
  }
}

void MotorControl::motorStopHB(void) {
  int j;
    for(j = 0; j < MAX_MOTORS; j++) {
      if(internalStatus[j].isRunning) {
        motorStopHB(j);
        if(tleCheckDiagnostic())
          tleDiagnostic(j, TLE_MOTOR_STOPPING);
      }
    }
}

void MotorControl::motorStopHB(int motor) {
  int hb1;

  // Calculate the first half bridge depending on the motor ID
  // Note that HB are numbered base 1 while motor is index array base 0
  // and HB usage is different if high current mode is selected in the
  // preprocessor directive
  hb1 = calcHB1(motor + 1);

  // Set motor stopped
  internalStatus[motor].isRunning = false;
  
  // Select the half bridges to configure the motor.
  // Depending on the mode (normal = 1 + 1 HB per motor, high current = 2+2)
  // not all the half bridges can be the first (=hb1)
  switch(hb1) {
    // -------------------------------------------------
    // Motor 1 (in both modes)
    // -------------------------------------------------
    case 1: 
      tle94112.configHB(tle94112.TLE_HB1, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
      tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
      #ifdef _HIGHCURRENT
      tle94112.configHB(tle94112.TLE_HB3,  tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
      tle94112.configHB(tle94112.TLE_HB4, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
      #endif
    break;
    // -------------------------------------------------
    // Motor 2 (no high current mode)
    // -------------------------------------------------
    case 3: 
      tle94112.configHB(tle94112.TLE_HB3, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
      tle94112.configHB(tle94112.TLE_HB4, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    break;
    // -------------------------------------------------
    // Motor 3 (Motor 2 in high current mode)
    // -------------------------------------------------
    case 5: 
      tle94112.configHB(tle94112.TLE_HB5, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
      tle94112.configHB(tle94112.TLE_HB6, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
      #ifdef _HIGHCURRENT
      tle94112.configHB(tle94112.TLE_HB7,  tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
      tle94112.configHB(tle94112.TLE_HB8, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
      #endif
    break;
    // -------------------------------------------------
    // Motor 4 (no high current mode)
    // -------------------------------------------------
    case 7:  
      tle94112.configHB(tle94112.TLE_HB7, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
      tle94112.configHB(tle94112.TLE_HB8, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    break;
    // -------------------------------------------------
    // Motor 5 ((Motor 3 in high current mode)
    // -------------------------------------------------
    case 9: 
      tle94112.configHB(tle94112.TLE_HB9, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
      tle94112.configHB(tle94112.TLE_HB10, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
      #ifdef _HIGHCURRENT
      tle94112.configHB(tle94112.TLE_HB11,  tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
      tle94112.configHB(tle94112.TLE_HB12, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
      #endif
    break;
    // -------------------------------------------------
    // Motor 6 (no high current mode)
    // -------------------------------------------------
    case 11:
      tle94112.configHB(tle94112.TLE_HB11, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
      tle94112.configHB(tle94112.TLE_HB12, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    break;
  }
}

void MotorControl::motorConfigHBCW(int motor) {
  int hb1;

  // Calculate the first half bridge depending on the motor ID
  // Note that HB are numbered base 1 while motor is index array base 0
  // and HB usage is different if high current mode is selected in the
  // preprocessor directive
  hb1 = calcHB1(motor + 1);

  // Set motor running
  internalStatus[motor].isRunning = true;
  
  // Select the half bridges to configure the motor.
  // Depending on the mode (normal = 1 + 1 HB per motor, high current = 2+2)
  // not all the half bridges can be the first (=hb1)
  switch(hb1) {
    // -------------------------------------------------
    // Motor 1 (in both modes)
    // -------------------------------------------------
    case 1: 
      #ifdef _HIGHCURRENT
      tle94112.configHB(tle94112.TLE_HB3,  tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      tle94112.configHB(tle94112.TLE_HB4, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      #else
      tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      #endif
      switch(internalStatus[motor].channelPWM){
        case tle94112.TLE_NOPWM:
          tle94112.configHB(tle94112.TLE_HB1, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
          #endif
        break;
        case tle94112.TLE_PWM1:
          tle94112.configHB(tle94112.TLE_HB1, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling);
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling);
          #endif
        break;
        case tle94112.TLE_PWM2:
          tle94112.configHB(tle94112.TLE_HB1, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling);
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling);
          #endif
        break;
        case tle94112.TLE_PWM3:
          tle94112.configHB(tle94112.TLE_HB1, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling);
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling);
          #endif
        break;
      }
    break;
    // -------------------------------------------------
    // Motor 2 (no high current mode)
    // -------------------------------------------------
    case 3: 
      tle94112.configHB(tle94112.TLE_HB4, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      switch(internalStatus[motor].channelPWM){
        case tle94112.TLE_NOPWM:
          tle94112.configHB(tle94112.TLE_HB3, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
        break;
        case tle94112.TLE_PWM1:
          tle94112.configHB(tle94112.TLE_HB3, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling);
        break;
        case tle94112.TLE_PWM2:
          tle94112.configHB(tle94112.TLE_HB3, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling);
        break;
        case tle94112.TLE_PWM3:
          tle94112.configHB(tle94112.TLE_HB3, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling);
        break;
      }
    break;
    // -------------------------------------------------
    // Motor 3 (Motor 2 in high current mode)
    // -------------------------------------------------
    case 5: 
      #ifdef _HIGHCURRENT
      tle94112.configHB(tle94112.TLE_HB7, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      tle94112.configHB(tle94112.TLE_HB8, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      #else
      tle94112.configHB(tle94112.TLE_HB6, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      #endif
      switch(internalStatus[motor].channelPWM){
        case tle94112.TLE_NOPWM:
          tle94112.configHB(tle94112.TLE_HB5, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB6, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
          #endif
        break;
        case tle94112.TLE_PWM1:
          tle94112.configHB(tle94112.TLE_HB5, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling);
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB6, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling);
          #endif
        break;
        case tle94112.TLE_PWM2:
          tle94112.configHB(tle94112.TLE_HB5, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling);
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB6, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling);
          #endif
        break;
        case tle94112.TLE_PWM3:
          tle94112.configHB(tle94112.TLE_HB5, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling);
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB6, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling);
          #endif
        break;
      }
    break;
    // -------------------------------------------------
    // Motor 4 (no high current mode)
    // -------------------------------------------------
    case 7:  
      tle94112.configHB(tle94112.TLE_HB8, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      switch(internalStatus[motor].channelPWM){
        case tle94112.TLE_NOPWM:
          tle94112.configHB(tle94112.TLE_HB7, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
        break;
        case tle94112.TLE_PWM1:
          tle94112.configHB(tle94112.TLE_HB7, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling);
        break;
        case tle94112.TLE_PWM2:
          tle94112.configHB(tle94112.TLE_HB7, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling);
        break;
        case tle94112.TLE_PWM3:
          tle94112.configHB(tle94112.TLE_HB7, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling);
        break;
      }
    break;
    // -------------------------------------------------
    // Motor 5 ((Motor 3 in high current mode)
    // -------------------------------------------------
    case 9: 
      #ifdef _HIGHCURRENT
      tle94112.configHB(tle94112.TLE_HB11, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      tle94112.configHB(tle94112.TLE_HB12, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      #else
      tle94112.configHB(tle94112.TLE_HB10, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      #endif
      switch(internalStatus[motor].channelPWM){
        case tle94112.TLE_NOPWM:
          tle94112.configHB(tle94112.TLE_HB9, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB10, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling );
          #endif
        break;
        case tle94112.TLE_PWM1:
          tle94112.configHB(tle94112.TLE_HB9, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling);
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB10, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling );
          #endif
        break;
        case tle94112.TLE_PWM2:
          tle94112.configHB(tle94112.TLE_HB9, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling);
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB10, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling );
          #endif
        break;
        case tle94112.TLE_PWM3:
          tle94112.configHB(tle94112.TLE_HB9, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling);
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB10, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling );
          #endif
        break;
      }
    break;
    // -------------------------------------------------
    // Motor 6 (no high current mode)
    // -------------------------------------------------
    case 11:
      tle94112.configHB(tle94112.TLE_HB12, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      switch(internalStatus[motor].channelPWM){
        case tle94112.TLE_NOPWM:
          tle94112.configHB(tle94112.TLE_HB11, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
        break;
        case tle94112.TLE_PWM1:
          tle94112.configHB(tle94112.TLE_HB11, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling);
        break;
        case tle94112.TLE_PWM2:
          tle94112.configHB(tle94112.TLE_HB11, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling);
        break;
        case tle94112.TLE_PWM3:
          tle94112.configHB(tle94112.TLE_HB11, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling);
        break;
      }
    break;
  }
}

void MotorControl::motorConfigHBCCW(int motor) {
  int hb1;

  // Calculate the first half bridge depending on the motor ID
  // Note that HB are numbered base 1 while motor is index array base 0
  // and HB usage is different if high current mode is selected in the
  // preprocessor directive
  hb1 = calcHB1(motor + 1);

  // Set motor stopped
  internalStatus[motor].isRunning = true;
  
  // Select the half bridges to configure the motor.
  // Depending on the mode (normal = 1 + 1 HB per motor, high current = 2+2)
  // not all the half bridges can be the first (=hb1)
  switch(hb1) {
    // -------------------------------------------------
    // Motor 1 (in both modes)
    // -------------------------------------------------
    case 1:
      tle94112.configHB(tle94112.TLE_HB1, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      #ifdef _HIGHCURRENT
      tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      #endif
      switch(internalStatus[motor].channelPWM){
        case tle94112.TLE_NOPWM:
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB3,  tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
          tle94112.configHB(tle94112.TLE_HB4, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
          #else
          tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
          #endif
        break;
        case tle94112.TLE_PWM1:
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB3,  tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling);
          tle94112.configHB(tle94112.TLE_HB4, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling);
          #else
          tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling);
          #endif
        break;
        case tle94112.TLE_PWM2:
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB3,  tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling);
          tle94112.configHB(tle94112.TLE_HB4, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling);
          #else
          tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling);
          #endif
        break;
        case tle94112.TLE_PWM3:
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB3,  tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling);
          tle94112.configHB(tle94112.TLE_HB4, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling);
          #else
          tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling);
          #endif
        break;
      }
    break;
    // -------------------------------------------------
    // Motor 2 (no high current mode)
    // -------------------------------------------------
    case 3: 
      tle94112.configHB(tle94112.TLE_HB3, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      switch(internalStatus[motor].channelPWM){
        case tle94112.TLE_NOPWM:
          tle94112.configHB(tle94112.TLE_HB4, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
        break;
        case tle94112.TLE_PWM1:
          tle94112.configHB(tle94112.TLE_HB4, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling);
        break;
        case tle94112.TLE_PWM2:
          tle94112.configHB(tle94112.TLE_HB4, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling);
        break;
        case tle94112.TLE_PWM3:
          tle94112.configHB(tle94112.TLE_HB4, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling);
        break;
      }
    break;
    // -------------------------------------------------
    // Motor 3 (Motor 2 in high current mode)
    // -------------------------------------------------
    case 5: 
      tle94112.configHB(tle94112.TLE_HB5, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      #ifdef _HIGHCURRENT
      tle94112.configHB(tle94112.TLE_HB6, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      #endif
      switch(internalStatus[motor].channelPWM){
        case tle94112.TLE_NOPWM:
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB7, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
          tle94112.configHB(tle94112.TLE_HB8, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
          #else
          tle94112.configHB(tle94112.TLE_HB6, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
          #endif
        break;
        case tle94112.TLE_PWM1:
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB7, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling);
          tle94112.configHB(tle94112.TLE_HB8, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling);
          #else
          tle94112.configHB(tle94112.TLE_HB6, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling);
          #endif
        break;
        case tle94112.TLE_PWM2:
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB7, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling);
          tle94112.configHB(tle94112.TLE_HB8, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling);
          #else
          tle94112.configHB(tle94112.TLE_HB6, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling);
          #endif
        break;
        case tle94112.TLE_PWM3:
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB7, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling);
          tle94112.configHB(tle94112.TLE_HB8, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling);
          #else
          tle94112.configHB(tle94112.TLE_HB6, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling);
          #endif
        break;
      }
    break;
    // -------------------------------------------------
    // Motor 4 (no high current mode)
    // -------------------------------------------------
    case 7:  
      tle94112.configHB(tle94112.TLE_HB7, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      switch(internalStatus[motor].channelPWM){
        case tle94112.TLE_NOPWM:
          tle94112.configHB(tle94112.TLE_HB8, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
        break;
        case tle94112.TLE_PWM1:
          tle94112.configHB(tle94112.TLE_HB8, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling);
        break;
        case tle94112.TLE_PWM2:
          tle94112.configHB(tle94112.TLE_HB8, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling);
        break;
        case tle94112.TLE_PWM3:
          tle94112.configHB(tle94112.TLE_HB8, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling);
        break;
      }
    break;
    // -------------------------------------------------
    // // Motor 5 ((Motor 3 in high current mode)
    // -------------------------------------------------
    case 9: 
      tle94112.configHB(tle94112.TLE_HB9, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      #ifdef _HIGHCURRENT
      tle94112.configHB(tle94112.TLE_HB10, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      #endif
      switch(internalStatus[motor].channelPWM){
        case tle94112.TLE_NOPWM:
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB11, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
          tle94112.configHB(tle94112.TLE_HB12, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling );
          #else
          tle94112.configHB(tle94112.TLE_HB10, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling );
          #endif
        break;
        case tle94112.TLE_PWM1:
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB11, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling);
          tle94112.configHB(tle94112.TLE_HB12, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling );
          #else
          tle94112.configHB(tle94112.TLE_HB10, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling );
          #endif
        break;
        case tle94112.TLE_PWM2:
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB11, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling);
          tle94112.configHB(tle94112.TLE_HB12, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling );
          #else
          tle94112.configHB(tle94112.TLE_HB10, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling );
          #endif
        break;
        case tle94112.TLE_PWM3:
          #ifdef _HIGHCURRENT
          tle94112.configHB(tle94112.TLE_HB11, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling);
          tle94112.configHB(tle94112.TLE_HB12, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling );
          #else
          tle94112.configHB(tle94112.TLE_HB10, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling );
          #endif
        break;
      }
    break;
    // -------------------------------------------------
    // Motor 6 (no high current mode)
    // -------------------------------------------------
    case 11: 
      tle94112.configHB(tle94112.TLE_HB11, tle94112.TLE_LOW, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
      switch(internalStatus[motor].channelPWM){
        case tle94112.TLE_NOPWM:
          tle94112.configHB(tle94112.TLE_HB12, tle94112.TLE_HIGH, tle94112.TLE_NOPWM, (uint8_t)internalStatus[motor].freeWheeling);
        break;
        case tle94112.TLE_PWM1:
          tle94112.configHB(tle94112.TLE_HB12, tle94112.TLE_HIGH, tle94112.TLE_PWM1, (uint8_t)internalStatus[motor].freeWheeling);
        break;
        case tle94112.TLE_PWM2:
          tle94112.configHB(tle94112.TLE_HB12, tle94112.TLE_HIGH, tle94112.TLE_PWM2, (uint8_t)internalStatus[motor].freeWheeling);
        break;
        case tle94112.TLE_PWM3:
          tle94112.configHB(tle94112.TLE_HB12, tle94112.TLE_HIGH, tle94112.TLE_PWM3, (uint8_t)internalStatus[motor].freeWheeling);
        break;
      }
    break;
  }
}

// ===============================================================
// Diagnostic methods
// ===============================================================

boolean MotorControl:: tleCheckDiagnostic(void) {
  if(tle94112.getSysDiagnosis() == tle94112.TLE_STATUS_OK)
    return false;
  else
    return true;
}

void MotorControl::tleDiagnostic(int motor, String message) {
  diagnosticHeader = message;
  tleDiagnostic(motor);
  diagnosticHeader = "";
}

void MotorControl::tleDiagnostic(int motor) {
  int diagnosis = tle94112.getSysDiagnosis();

  if(diagnosis == tle94112.TLE_STATUS_OK) {
    Serial << diagnosticHeader << " Motor " << motor << " - " << TLE_NOERROR << endl;
  } // No errors
  else {
    #ifndef _IGNORE_OPENLOAD
    if(tle94112.getSysDiagnosis(tle94112.TLE_LOAD_ERROR) != 0) {
      Serial << diagnosticHeader << " Motor " << motor << " - " << TLE_ERROR_MSG << endl;
      Serial << TLE_LOADERROR << endl;
    } // Open load error
    #endif
    if(tle94112.getSysDiagnosis(tle94112.TLE_SPI_ERROR) != 0) {
      Serial << diagnosticHeader << " Motor " << motor << " - " << TLE_ERROR_MSG << endl;
      Serial << TLE_SPIERROR << endl;
    }
    if(tle94112.getSysDiagnosis(tle94112.TLE_UNDER_VOLTAGE) != 0) {
      Serial << diagnosticHeader << " Motor " << motor << " - " << TLE_ERROR_MSG << endl;
      Serial << TLE_UNDERVOLTAGE << endl;
    }
    if(tle94112.getSysDiagnosis(tle94112.TLE_OVER_VOLTAGE) != 0) {
      Serial << diagnosticHeader << " Motor " << motor << " - " << TLE_ERROR_MSG << endl;
      Serial <<TLE_OVERVOLTAGE << endl;
    }
    if(tle94112.getSysDiagnosis(tle94112.TLE_POWER_ON_RESET) != 0) {
      Serial << diagnosticHeader << " Motor " << motor << " - " << TLE_ERROR_MSG << endl;
      Serial << TLE_POWERONRESET << endl;
    }
    if(tle94112.getSysDiagnosis(tle94112.TLE_TEMP_SHUTDOWN) != 0) {
      Serial << diagnosticHeader << " Motor " << motor << " - " << TLE_ERROR_MSG << endl;
      Serial << TLE_TEMPSHUTDOWN << endl;
    }
    if(tle94112.getSysDiagnosis(tle94112.TLE_TEMP_WARNING) != 0) {
      Serial << diagnosticHeader << " Motor " << motor << " - " << TLE_ERROR_MSG << endl;
      Serial << TLE_TEMPWARNING;
    }
    // Clear all possible error conditions        
    tle94112.clearErrors();
    diagnosticHeader = "";
  } // Error condition
}

void MotorControl::tleDiagnostic() {
  int diagnosis = tle94112.getSysDiagnosis();

  if(diagnosis == tle94112.TLE_STATUS_OK) {
    Serial << diagnosticHeader << TLE_NOERROR << endl;
  } // No errors
  else {
    diagnosticHeader += TLE_ERROR_MSG;
    #ifndef _IGNORE_OPENLOAD
    if(tle94112.getSysDiagnosis(tle94112.TLE_LOAD_ERROR) != 0) {
      Serial << diagnosticHeader << endl;
      Serial << TLE_LOADERROR << endl;
    } // Open load error
    #endif
    if(tle94112.getSysDiagnosis(tle94112.TLE_SPI_ERROR) != 0) {
      Serial << diagnosticHeader << endl;
      Serial << TLE_SPIERROR << endl;
    }
    if(tle94112.getSysDiagnosis(tle94112.TLE_UNDER_VOLTAGE) != 0) {
      Serial << diagnosticHeader << endl;
      Serial << TLE_UNDERVOLTAGE << endl;
    }
    if(tle94112.getSysDiagnosis(tle94112.TLE_OVER_VOLTAGE) != 0) {
      Serial << diagnosticHeader << endl;
      Serial <<TLE_OVERVOLTAGE << endl;
    }
    if(tle94112.getSysDiagnosis(tle94112.TLE_POWER_ON_RESET) != 0) {
      Serial << diagnosticHeader << endl;
      Serial << TLE_POWERONRESET << endl;
    }
    if(tle94112.getSysDiagnosis(tle94112.TLE_TEMP_SHUTDOWN) != 0) {
      Serial << diagnosticHeader << endl;
      Serial << TLE_TEMPSHUTDOWN << endl;
    }
    if(tle94112.getSysDiagnosis(tle94112.TLE_TEMP_WARNING) != 0) {
      Serial << diagnosticHeader << endl;
      Serial << TLE_TEMPWARNING;
    }
    // Clear all possible error conditions        
    tle94112.clearErrors();
    diagnosticHeader = " ";
  } // Error condition
}

// ===============================================================
// Dump system configuration to serial
// ===============================================================

void MotorControl::showInfo(void) {
  int j;
  // Motor table header
  Serial << INFO_MAIN_HEADER1 << endl << INFO_MOTORS_TITLE << endl << INFO_MAIN_HEADER1 << endl;
  Serial << INfO_TAB_HEADER2 << endl << INfO_TAB_HEADER1 << endl << INfO_TAB_HEADER2 << endl;
  // Build the motors settings table data
  for (j = 0; j < MAX_MOTORS; j++) {
    // #1 - Motor
    Serial << INFO_FIELD1A << (j + 1) << INFO_FIELD1B;
    // #2 - Enabled
    if(internalStatus[j].isEnabled)
      Serial << INFO_FIELD2Y;
    else
      Serial << INFO_FIELD2N;
    // #3 - Active freewheeling
    if(internalStatus[j].freeWheeling)
      Serial << INFO_FIELD4Y;
    else
      Serial << INFO_FIELD4N;
    // #4 - Direction
    if(internalStatus[j].motorDirection == MOTOR_DIRECTION_CW)
      Serial << INFO_FIELD8A;
    else
      Serial << INFO_FIELD8B;
    // #5 - PWM
    switch(internalStatus[j].channelPWM) {
      case tle94112.TLE_NOPWM:
        Serial << INFO_FIELD9_NO;
      break;
      case tle94112.TLE_PWM1:
        Serial << INFO_FIELD9_80;
      break;
      case tle94112.TLE_PWM2:
        Serial << INFO_FIELD9_100;
      break;
      case tle94112.TLE_PWM3:
        Serial << INFO_FIELD9_200;
      break;
    }
    Serial << endl << INfO_TAB_HEADER2 << endl;
  }

  // PWM table header
  Serial << endl << INFO_MAIN_HEADER2 << endl << INFO_PWM_TITLE << endl << INFO_MAIN_HEADER2 << endl;
  Serial << INfO_TAB_HEADER4 << endl << INfO_TAB_HEADER3 << endl << INfO_TAB_HEADER4 << endl;
  // Build the pwm settings table data
  for (j = 0; j < AVAIL_PWM_CHANNELS; j++) {
    // #1 - PWM
    switch(j) {
      case 0:
        Serial << INFO_FIELD10_80;
      break;
      case 1:
        Serial << INFO_FIELD10_100;
      break;
      case 2:
        Serial << INFO_FIELD10_200;
      break;
    }
    // #2 - DC Min
    Serial << INFO_FIELD5_6A;
    if(dutyCyclePWM[j].minDC < 10)
      Serial << "  ";
    else 
      if(dutyCyclePWM[j].minDC < 100)
        Serial << " ";
    Serial << dutyCyclePWM[j].minDC << INFO_FIELD5_6B;
    // #3 - DC Max
    Serial << INFO_FIELD5_6A;
    if(dutyCyclePWM[j].maxDC < 100)
      Serial << " ";
    Serial << dutyCyclePWM[j].maxDC << INFO_FIELD5_6B;
    // #4 - Manual DC
    if(dutyCyclePWM[j].manDC)
      Serial << INFO_FIELD7Y;
    else
      Serial << INFO_FIELD7N;
    // #5 - Acceleration
    if(dutyCyclePWM[j].useRamp)
      Serial << INFO_FIELD3Y;
    else
      Serial << INFO_FIELD3N;
    
    Serial << endl << INfO_TAB_HEADER4 << endl;
  }
}





