#ifndef MOTORS_C
#define MOTORS_C

#include <lpc213x.h>
#include <stdlib.h>
#include <stdio.h>
#include "target.h"
#include "Declaraciones.h"
#include "motors.h"
#include "s1d15g00.h"

/********************** Obtención de velocidad angular **********************/
float get_W(void)
{
	long sensors_average = 0;
	int sensors_sum = 0;
	float position = 0;
	float last_proportional = proportional;
	float derivative = 0;
	float w_ctrl = 0;

	int i = 0;
	negros = 0;

	for(i = 0; i < S; i++)
	{
		if(color_suelo == NEGRO)
			ADCresult[i] = blanco[i] + (negro[i] - ADCresult[i]);

		if(ADCresult[i] < 100)
			ADCresult[i] = blanco[i];
		else if(ADCresult[i] > umbral[i])
			negros++;

		sensors_average += ADCresult[i]*(i+1);
		sensors_sum += ADCresult[i];
	}

	if(negros == 0)
	{
		if(giro == IZQ)		// Giro a izquierdas
			position = 1;
		else // giro == DER	// Giro a derechas
			position = S;
	}
	else // sensors_sum != 0
		position = (float) sensors_average / sensors_sum;

	proportional = position - (S+1)/2;

	if(lin == '3')
	{
		if(proportional < 0)
			lin = IZQ;
		else
			lin = DER;
	}

	if(negros != 0)
	{
		if(proportional < (float)(-(S/2 + 1)/2))
		{
			giro_ant = giro;
			giro = IZQ;
		}
		else if(proportional > (float)(S/2 + 1)/2)
		{
			giro_ant = giro;
			giro = DER;
		}
	}

	integral = integral + proportional;
	if(integral > integral_max)
		integral = integral_max;
	else if(integral < -integral_max)
		integral = -integral_max;

	derivative = proportional - last_proportional;

	w_ctrl = (float)(proportional * Kp + integral * Ki + derivative * Kd);

	return w_ctrl;
}

/********************** Obtención de velocidad lineal **********************/
float get_VL(void)
{
	static float vl = 30;

	if(vl < v_rapida)
	{
		vl += 0.1;

		if(vl > v_rapida)
			vl = v_rapida;
	}


/*

 //obtener consigna de velocidad lineal a partir del reconocimiento del circuito

*/

	return vl;
}

/********************** Asignación de velocidad a los motores **********************/
void set_motors(float vl, float w_ctrl)
{
	float v_izq = 0, v_der = 0;

	switch(ESTADO_RASTREO)
	{
		case Rastreo_cambio_lin:
			if(lin == IZQ)
				w_ctrl = 1;//0.5;
			else
				w_ctrl = -1;//-0.5;

			if(negros == 0)
				cont_cambio_lin += PER_PERIOD;

			break;
		case Rastreo_avanzar:
			w_ctrl = 0;

			break;
		case Rastreo_normal:
			break;			
	}			

	if(w_ctrl > w_ctrl_max)
		w_ctrl = w_ctrl_max;
	else if(w_ctrl < -w_ctrl_max)
		w_ctrl = -w_ctrl_max;

	if(w_ctrl < -w_ctrl_max/2)
	{
		v_izq = (-w_ctrl_max/2 - w_ctrl/2)/(w_ctrl_max/2) * vl;
		v_der = (w_ctrl_max/2 - w_ctrl/2)/(w_ctrl_max/2) * vl;

		mode_izq = LOCKED;
		mode_der = FORWARD;
	}
	else if(w_ctrl > w_ctrl_max/2)
	{
		v_izq = (w_ctrl_max/2 + w_ctrl/2)/(w_ctrl_max/2) * vl;
		v_der = (-w_ctrl_max/2 + w_ctrl/2)/(w_ctrl_max/2) * vl;
	
		mode_izq = FORWARD;
		mode_der = LOCKED;
	}
	else
	{
		v_izq = (w_ctrl_max/2 + w_ctrl/2)/(w_ctrl_max/2) * vl;
		v_der = (w_ctrl_max/2 - w_ctrl/2)/(w_ctrl_max/2) * vl;
	
		mode_izq = FORWARD;
		mode_der = FORWARD;
	}

	/* Slew Rate */
	if(v_izq > 100)
		v_izq = 100;
	else if(v_izq < 0)
		v_izq = 0;
	if(v_der > 100)
		v_der = 100;
	else if(v_der < 0)
		v_der = 0;

	MOTOR_SPEED_set(MOTOR_IZQ, v_izq, mode_izq);
	MOTOR_SPEED_set(MOTOR_DER, v_der, mode_der);

	PWMLER = (1<<0)|(1<<MI_PWM)|(1<<MD_PWM);
}


#endif
