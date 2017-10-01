/* 
 *  \file motor.h
 *  \brief DC motor parameters for the Infineon DC motor controller Arduino shield
 *  TLE94112 from Infineon
 *  
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics sl <www.balearicdynamics.com> SPAIN
 *  \date September 2017
 *  \version 0.1
 *  Licensed under GNU LGPL 3.0
 */

#ifndef _MOTOR
#define _MOTOR

//! Application title shown on startup and after reset
#define APP_TITLE "CM3 I2C PONF Ver.0.0.6"

#define _HIGHCURRENT
#undef _MOTORDEBUG

//! Avoid too many openload error messages when starting acceleration
//! by default it is ignored
#define _IGNORE_OPENLOAD

#define INVERT_DIRECTION_DELAY 50  ///< Delay in ms when the motor should invert direction
#define ACCELERATION_DELAY 2        ///< Delay between acceleration steps

#define MOTOR_DIRECTION_CW 1      ///< Clockwise
#define MOTOR_DIRECTION_CCW 2     ///< Counterclockwise

#define MOTOR_FW_ACTIVE true       ///< Active freewheeling
#define MOTOR_FW_PASSIVE false     ///< Passive freewheeling

#define MOTOR_MANUAL_DC true      ///< Manual motor duty cycle target control
#define MOTOR_AUTO_DC false       ///< Motor duty cycle based on the internal settings

#define MOTOR_ENABLED 1
#define MOTOR_DISABLED 0

#define RAMP_ON true     ///< Acceleration enabled on start
#define RAMP_OFF false    ///< Acceleration disabled on start

#define MOTOR_PWM_ON 1  ///< Motor run with PWM
#define MOTOR_PWM_OFF 0 ///< Motor runs without PWM

#define DUTYCYCLE_MIN 0     ///< Minimum duty cycle for motor start. Depends on motor characteristics
#define DUTYCYCLE_MAX 255   ///< Maximum duty cycle
#define RAMP_STEP_DELAY 2   ///< Delay (ms) between steps during an acceleration/deceleration cycle

#define AVAIL_PWM_CHANNELS 3  ///< Number of available PWM channels (excluding the NOPWM mode)
#define PWM80_CHID 1          ///< ID for PWM channel 80 Hz
#define PWM100_CHID 2         ///< ID for PWM channel 100 Hz
#define PWM200_CHID 3         ///< ID for PWM channel 200 Hz

/**
 * When _HIGHCURRENT is set every motor needs 2+2 half bridges to double the needed power
 */
#undef _HIGHCURRENT

#ifdef _HIGHCURRENT
  //! In high current mode every pole of the motors is connected to two half bridges
  #define MAX_MOTORS 3
  //! Calculate the first half bridge number depending
  //! on the motor number
  #define calcHB1(x) ((x - 1) * 4 + 1)
#else
  //! In normal mode every motor uses two half bridges
  #define MAX_MOTORS 6
  //! Calculate the first half bridge number depending
  //! on the motor number
  #define calcHB1(x) ((x - 1) * 2 + 1)
#endif

// ======================================================================
//        Generic Strings
// ======================================================================

//! Error title
#define TLE_ERROR_MSG "TLE94112 Diagnostic Status :"
#define TLE_NOERROR "No Errors"
#define TLE_SPIERROR "SPI communication"
#define TLE_LOADERROR "Open Load"
#define TLE_UNDERVOLTAGE "Under Voltage"
#define TLE_OVERVOLTAGE "Over Voltage"
#define TLE_POWERONRESET "Power Reset" 
#define TLE_TEMPSHUTDOWN "Temp shutdown"
#define TLE_TEMPWARNING "Warning too hot"

#define TLE_MOTOR_STARTING "Starting"
#define TLE_MOTOR_STOPPING "Stopping"
#define TLE_MOTOR_HALT "Halted"
#define TLE_MOTOR_RUN "Running"

// ======================================================================
//        LCD String
// ======================================================================

//! Application title shown on startup
#define L_APP_NAME1 "Infineon test"
#define L_APP_NAME2 "TLE94112 Shield"

// ======================================================================
//        Configuration info constants
// ======================================================================

#define INFO_MAIN_HEADER1     "*********************************"
#define INFO_MAIN_HEADER2     "*************************************"
#define INFO_MOTORS_TITLE     "      Motors configuration"
#define INFO_PWM_TITLE        "       PWM Channels settings"
#define INfO_TAB_HEADER1      "|Motor|Enabled|Active FW|Dir|PWM|"
#define INfO_TAB_HEADER2      "|-----+-------+---------+---+---|"
#define INfO_TAB_HEADER3      "|PWM Chan|DC Min|DC Max|DC Man|Accel|"
#define INfO_TAB_HEADER4      "|--------+------+------+------+-----|"

// Motor num
#define INFO_FIELD1A "| M"
#define INFO_FIELD1B "  |"

#define INFO_FIELD2Y "  Yes  |"
#define INFO_FIELD2N "   No  |"

#define INFO_FIELD3Y " Yes |"
#define INFO_FIELD3N "  No |"

#define INFO_FIELD4Y "   Yes   |"
#define INFO_FIELD4N "    No   |"

#define INFO_FIELD5_6A " "
#define INFO_FIELD5_6B "  |"

#define INFO_FIELD7Y "  Yes |"
#define INFO_FIELD7N "   No |"

#define INFO_FIELD8A " CW|"
#define INFO_FIELD8B "CCW|"

#define INFO_FIELD9_NO " No|"
#define INFO_FIELD9_80 " 80|"
#define INFO_FIELD9_100 "100|"
#define INFO_FIELD9_200 "200|"

#define INFO_FIELD10_80 "|  80 Hz |"
#define INFO_FIELD10_100 "| 100 Hz |"
#define INFO_FIELD10_200 "| 200 Hz |"

#endif
