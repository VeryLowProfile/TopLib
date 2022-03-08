/*
  ******************************************************************************
  * @file           : scan_PID.c
  * @author         : Davide
  * @date           : Aug 27, 2021
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "scan_PID.h"
#include "main.h"

/* Declarations -----------------------------------------------------------*/
PID Pid[PID_NUMBER];	//declaration of PID channels

/* Functions -----------------------------------------------------------*/
void Scan_PID_Init (uint8_t PidNumber, float Kp, float Ki, float Kd, float OutLimMin, float OutLimMax, float intLimMin, float intLimMax, float T)
{
	Pid[PidNumber].Kp = Kp;
	Pid[PidNumber].Ki = Ki;
	Pid[PidNumber].Kd = Kd;

	Pid[PidNumber].limMin = OutLimMin; //output clamping
	Pid[PidNumber].limMax = OutLimMax;

	Pid[PidNumber].limMinInt = intLimMin; //integrator clamping
	Pid[PidNumber].limMaxInt = intLimMax;

	Pid[PidNumber].T = T; // sampling time = 1/fs
	Pid[PidNumber].tau = (Pid[PidNumber].T) * 2; //filtering time constant for derivative term
}

void Scan_PID_Update_K (uint8_t PidNumber, float Kp, float Ki, float Kd)
{
	Pid[PidNumber].Kp = Kp;
	Pid[PidNumber].Ki = Ki;
	Pid[PidNumber].Kd = Kd;
}

float Scan_PID_Process(uint8_t PidNumber, float setpoint, float ref)
{

	//Error signal
    float error = setpoint - ref;

	// Proportional
    float proportional = Pid[PidNumber].Kp * error;

	// Integral
    Pid[PidNumber].integrator = Pid[PidNumber].integrator + 0.5f * Pid[PidNumber].Ki * Pid[PidNumber].T * (error + Pid[PidNumber].prevError);

	// Anti wind up
    if (Pid[PidNumber].integrator > Pid[PidNumber].limMaxInt) {

    	Pid[PidNumber].integrator = Pid[PidNumber].limMaxInt;

    } else if (Pid[PidNumber].integrator < Pid[PidNumber].limMinInt) {

    	Pid[PidNumber].integrator = Pid[PidNumber].limMinInt;
    }

	// Derivative (band - limited differentiator)
    if(Pid[PidNumber].differentiator != 0){
    	Pid[PidNumber].differentiator = -(2.0 * Pid[PidNumber].Kd * (ref - Pid[PidNumber].prevMeasurement)	/* Note: derivative on measurement, therefore minus sign in front of equation! */
						+ (2.0 * Pid[PidNumber].tau - Pid[PidNumber].T) * Pid[PidNumber].differentiator)
						/ (2.0 * Pid[PidNumber].tau + Pid[PidNumber].T);
    }

	// Compute output and apply limits
    Pid[PidNumber].out = proportional + Pid[PidNumber].integrator + Pid[PidNumber].differentiator;

    if (Pid[PidNumber].out > Pid[PidNumber].limMax) {

    	Pid[PidNumber].out = Pid[PidNumber].limMax;

    } else if (Pid[PidNumber].out < Pid[PidNumber].limMin) {

    	Pid[PidNumber].out = Pid[PidNumber].limMin;

    }

	/* Store error and measurement for later use */
    Pid[PidNumber].prevError       = error;
    Pid[PidNumber].prevMeasurement = ref;

	/* Return controller output */
    return Pid[PidNumber].out;

}
