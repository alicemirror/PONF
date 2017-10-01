/**
 *  \file shutter.h
 *  \brief ShutterControl definitions
 *  
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics sl <www.balearicdynamics.com> SPAIN
 *  \date September 2017
 *  \version 0.1
 *  Licensed under GNU LGPL 3.0 
 */

#ifndef _SHUTTER
#define _SHUTTER

//! Shutter top window signal
#define SH_TOP 5
//! Shutter bottom window signal
#define SH_BOTTOM 6
//! Shutter motor ID
#define SH_MOTOR 1
//! Autofocus motor
#define AF_MOTOR 2
//! Zoom motor
#define Z_MOTOR 3
//! Motor cycle duration (ms)
#define SH_MOTOR_MS 25
//! PWM Min/Max duty cycle for the AF and ZOOM motors
#define AF_MIN_DC 32
#define AF_MAX_DC 128
#define Z_MIN_DC 50
#define Z_MAX_DC 255




#endif
