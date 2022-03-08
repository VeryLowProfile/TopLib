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
	pid[PidNumber].Kp = Kp;
	pid[PidNumber].Ki = Ki;
	pid[PidNumber].Kd = Kd;

	pid[PidNumber].limMin = OutLimMin; //output clamping
	pid[PidNumber].limMax = OutLimMax;

	pid[PidNumber].limMinInt = intLimMin; //integrator clamping
	pid[PidNumber].limMaxInt = intLimMax;

	pid[PidNumber].T = T; // sampling time = 1/fs
	pid[PidNumber].tau = (pid[PidNumber].T) * 2; //filtering time constant for derivative term
}

void Scan_PID_Update_K (uint8_t PidNumber, float Kp, float Ki, float Kd)
{
	pid[PidNumber].Kp = Kp;
	pid[PidNumber].Ki = Ki;
	pid[PidNumber].Kd = Kd;
}

float Scan_PID_Process(uint8_t PidNumber, float setpoint, float ref)
{

	//Error signal
    float error = setpoint - ref;

	// Proportional
    float proportional = pid[PidNumber].Kp * error;

	// Integral
    pid[PidNumber].integrator = pid[PidNumber].integrator + 0.5f * pid[PidNumber].Ki * pid[PidNumber].T * (error + pid[PidNumber].prevError);

	// Anti wind up
    if (pid[PidNumber].integrator > pid[PidNumber].limMaxInt) {

    	pid[PidNumber].integrator = pid[PidNumber].limMaxInt;

    } else if (pid[PidNumber].integrator < pid[PidNumber].limMinInt) {

    	pid[PidNumber].integrator = pid[PidNumber].limMinInt;
    }

	// Derivative (band - limited differentiator)
    if(pid[PidNumber].differentiator != 0){
    	pid[PidNumber].differentiator = -(2.0 * pid[PidNumber].Kd * (ref - pid[PidNumber].prevMeasurement)	/* Note: derivative on measurement, therefore minus sign in front of equation! */
						+ (2.0 * pid[PidNumber].tau - pid[PidNumber].T) * pid[PidNumber].differentiator)
						/ (2.0 * pid[PidNumber].tau + pid[PidNumber].T);
    }

	// Compute output and apply limits
    pid[PidNumber].out = proportional + pid[PidNumber].integrator + pid[PidNumber].differentiator;

    if (pid[PidNumber].out > pid[PidNumber].limMax) {

    	pid[PidNumber].out = pid[PidNumber].limMax;

    } else if (pid[PidNumber].out < pid[PidNumber].limMin) {

    	pid[PidNumber].out = pid[PidNumber].limMin;

    }

	/* Store error and measurement for later use */
    pid[PidNumber].prevError       = error;
    pid[PidNumber].prevMeasurement = ref;

	/* Return controller output */
    return pid[PidNumber].out;

}
