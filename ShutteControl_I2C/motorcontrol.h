/**
 *  \file motorcontrol.h
 *  \brief Arduino class to control Infineon DC Motor Shield with TLE94112
 *  
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics sl <www.balearicdynamics.com> SPAIN
 *  \date July 2017, update September 2017
 *  \version 2.0
 *  Licensed under GNU LGPL 3.0 
 */

#ifndef _MOTORCONTROL
#define _MOTORCONTROL

#include <Streaming.h>
#include <TLE94112.h>
#include "motor.h"

/**
 * All the state flas and value settings for a generic motor
 */
struct motorStatus {
  uint8_t channelPWM;     ///< PWM channel for this motor
  boolean isEnabled;      ///< Motor enabled status
  boolean isRunning;      ///< Motor running status (should be enabled)
  boolean freeWheeling;   ///< Free wheeling active or passive
  int motorDirection;     ///< Current motor direction
};

/**
 * PWM duty cycle settings. All motors using the same
 * PWM channel will be affected by the same settings
 */
struct pwmStatus {
  boolean useRamp;        ///< Use acceleration/deceleration cycle when starting
  uint8_t minDC;          ///< Min duty cycle value
  uint8_t maxDC;          ///< Max duty cycle value
  boolean manDC;          ///< Manual duty cycle flag
};

/**
 * \brief  Class to control the TLE94112 Arduino shield
 * 
 * The class control three PWM channels and two ranges of motors.
 * If _HIGH_CURRENT is defined are used two half bridges every motor pole
 * so only 3 max motors are allowed else 6 motors can be managed
 * 
 * In this class we define an hardcoded assumption presetting the three PWM 
 * frequencies associated by default to the three channels. 
 * A different frequency can be associated
 * to every motor selecting the corresponding PWM channel ID.\n
 * With this choice we loose the option to assgin the same frequency to two
 * different PWM channels and different duty cycles but it a meaningless
 * limitation that can be considered acceptable considering the flexibility
 * of the entire class.
 */
class MotorControl {
  public:

    //! Current motor in use (setting parameters)
    int currentMotor;
    //! Current PWM channel (setting parameters)
    int currentPWM;
    //! Status of the motors parameters and settings
    motorStatus internalStatus[MAX_MOTORS];
    //! Status of the PWM duty cycle
    pwmStatus dutyCyclePWM[AVAIL_PWM_CHANNELS];
    //! Compound diagnostic string. Used when motor number is available
    String diagnosticHeader;
    //! The last duty cycle value read from the analog input (manual duty cycle settings)
    uint8_t lastAnalogDC;
    //! The previous duty cycle value read from the analog input (manual duty cycle settings)
    uint8_t prevAnalogDC;
    //! Global flag is one (or more) of the PWM channels are set to manualDC
    boolean hasManualDC;

    /** 
     * \brief Initialization and motor settings 
     * 
     * The initialization method support two hardcoded modes, depending on the
     * kind of geared motor it is used.\n
     * In _HIGHCURRENT mode every motor uses two half bridges couple together for every 
     * pole if more than 0.9A is needed (< 0.18)\n
     * The standard usage mode is in low current mode with a single half bridge every motor pole
     */
    void begin(void);

    //! \brief stop the motor control
    void end(void);

    /**
     * \brief initialize the motor default settings and disable all the motors
     * Launched when the class is initialised.
     */
    void reset(void);

    /**
     * \brief Reset all the half bridges immediately stopping the motors
     */
    void resetHB(void);

    /**
     * \brief Reset the PWM channels setting the duty cycle to 0
     * 
     * \note This method is also used in the initialization phase of the
     * start command to avoid that the motors with PWM assigned will star
     * before the end of the start settings.
     */
    void resetPWM(void);

    /**
     * \brief Set the desired PWM channel to the current motor if one
     * or to all motors
     * 
     * \param pwmCh The selected PWM channel
     */
    void setPWM(uint8_t pwmCh);

     /**
      * \brief Set the desired direction for the selected motor (or all)
      * 
      * \param dir The selected direction, CW or CCW
      */
    void setMotorDirection(int dir);

    /**
     * \brief Enable or disable the acceleration/deceleration sequence
     * for the desired PWM channel
     * 
     * \param acc Acceleration flag
     */
    void setPWMRamp(boolean acc);

    /**
     * \brief Start PWM channels
     */
    void motorPWMStart(void);

    /**
     * \brief Start PWM channels
     */
    void motorPWMStop(void);
    
    /**
     * \brief Run PWM channels with an acceleration cycle
     * 
     * \param channel the selectedPWM channel
     */
    void motorPWMAccelerate(int channel);
    
    /**
     * \brief Halt PWM channels with a deceleration cycle
     * 
     * \param channel the selectedPWM channel
     */
    void motorPWMDecelerate(int channel);

    /**
     * \brief Change the current duty cicle value through acceleration/deceleration
     * for the PWM channels that has set the manual duty cycle
     */
    void motorPWMAnalogDC(void);

    /**
     * \bruief Run PWM channel immediately setting the max duty cycle
     * 
     * \param channel the selectedPWM channel
     */
    void motorPWMRun(int channel);

    /**
     * \brief Halt PWM channel immediately setting the duty cycle to 0
     * 
     * \param channel the selectedPWM channel
     */
    void motorPWMHalt(int channel);
    
    /**
     * Enable or disable the freewheeling flag
     * 
     * \param fw Freewheeling flag
     */
    void setMotorFreeWheeling(boolean fw);

    /**
     * \brief Set the state flag for duty cycle mode. 
     * 
     * If set to manual the target value (max) is read 
     * from the analog input (pot) else it
     * is set following the internal parameters values.
     * This method applies to any iof the three available
     * PWM channels
     * 
     * \param dc true = manual duty cycle while runnning, 
     * false = use the preset values
     */
    void setPWMManualDC(boolean dc);

    /**
     * \brief Assign a user defined value as min duty cycle
     * 
     * This method applies to any of the three available
     * PWM channels
     * 
     * \param dc Duty cycle minimum value for the selected PWM channel
     */
    void setPWMMinDC(uint8_t dc);

    /**
     * \brief Assign a user defined value as Max duty cycle
     * 
     * This method applies to any of the three available
     * PWM channels
     * 
     * \param dc Duty cycle maximum value for the selected PWM channel
     */
    void setPWMMaxDC(uint8_t dc);

    /**
     * \brief Start all enabled motors
     */
    void startMotors();

    /**
     * \brief Stop all running motors
     */
    void stopMotors();

    /**
     * \brief Start the selected motor
     * 
     * \param m The motor number (1-6)
     */
    void startMotor(int m);

    /**
     * \brief Stop the selected motor
     * 
     * \param m The motor number (1-6)
     */
    void stopMotor(int m);

    /**
     * \brief Configure the halfbridges of all the motors. 
     * 
     * This method should be called during start global command.
     * The method loop on all motors and call recursively the same
     * polymorphic method that exectues the real setup for the motor
     * based on the direction.
     */
    void motorConfigHB(void);

    /**
     * \brief Configure the halfbridges of the specified motor. 
     * 
     * This method should be called to setup a single motor.
     * If the selected motor is not enabled the command has no effect else
     * it calls the motorConfigHBCW() or motorConfigHBCW() according with the
     * motor selected direction.
     * 
     * \param motor The motor ID to be configured (base 0)
     */
    void motorConfigHB(int motor);

    /**
     * \brief Configure the halfbridges of the specified motor, clowckwise direction
     * 
     * Thie method setup the HB the specified motor according with
     * its currently settings and parameters configured in the internalStatus[] array
     * 
     * \param motor The motor ID to be configured (base 0)
     */
    void motorConfigHBCW(int motor);

    /**
     * \brief Configure the halfbridges of the specified motor, counterclowckwise direction
     * 
     * This method setup the HB the specified motor according with
     * its currently settings and parameters configured in the internalStatus[] array
     * 
     * \param motor The motor ID to be configured (base 0)
     */
    void motorConfigHBCCW(int motor);

    /*
     * \brief Stop all running motors
     * 
     * This method stops immediately all running motors resetting the half bridges
     * and deactivating the PWM if active
     */
    void motorStopHB(void);

    /*
     * \brief Stop the specified motor
     * 
     * This method stops immediately the selected motor resetting the half bridges
     * and deactivating the PWM if active
     */
    void motorStopHB(int motor);

    /** 
     * \brief Show Current motors configuration in a table and the PWM settings on
     * another to the serial terminal
     */
     void showInfo(void);

    /**
     * Check if an error occured.
     * 
     * \note This method should be used for test the error condition only as it does not
     * 
     * \return true if tehre is an error
     */
    boolean tleCheckDiagnostic(void);

    /**
     * Check the error condition and detect the kind of error (if any) then reset it
     * 
     * \return The error string
     * \todo Check the harfbridge generating the specific error 
     */
    void tleDiagnostic(void);

    /**
     * Check the error condition and detect the kind of error (if any) then reset it
     * 
     * \ param motor The motor ID (base 0) that has generated the error
     */
    void tleDiagnostic(int motor);

    /**
     * Check the error conditions and detect the kind of error (if any) then reset it
     * 
     * \param motor The motor ID (base 0) that has generated the error
     * \param message A generic string message for better explanation
     */
    void tleDiagnostic(int motor, String message);

};

#endif
