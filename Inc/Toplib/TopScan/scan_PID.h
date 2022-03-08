/**
  ******************************************************************************
  * @file           : scan_PID.h
  * @author         : Davide
  * @date           : 27 ago 2021
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "genericDefines.h"
#include "stm32g4xx_hal.h"

/* Defines -----------------------------------------------------------*/
#define PID_NUMBER 4

/* Declarations -----------------------------------------------------------*/
typedef struct {

	/* Controller gains */
	float Kp;
	float Ki;
	float Kd;

	/* Derivative low-pass filter time constant */
	float tau;

	/* Output limits */
	float limMin;
	float limMax;

	/* Integrator limits */
	float limMinInt;
	float limMaxInt;

	/* Sample time (in seconds) */
	float T;

	/* Controller "memory" */
	float integrator;
	float prevError;			/* Required for integrator */
	float differentiator;
	float prevMeasurement;		/* Required for differentiator */

	/* Controller output */
	float out;

} PID;

/* Prototypes -----------------------------------------------------------*/
void Scan_PID_Init (int8_t PidNumber, float Kp, float Ki, float Kd, float OutLimMin, float OutLimMax, float intLimMin, float intLimMax, float T);
void Scan_PID_Update_K (uint8_t PidNumber, float Kp, float Ki, float Kd);
float Scan_PID_Process(PID *pid, float setpoint, float measurement);

