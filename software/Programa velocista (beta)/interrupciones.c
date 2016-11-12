#ifndef INTERRUPCIONES_C
#define INTERRUPCIONES_C

#include <lpc213x.h>
#include <stdlib.h>
#include <stdio.h>
#include "LPClib.h"
#include "target.h"
#include "declaraciones.h"
#include "motors.h"


void EINT_0(void)__irq
{
}

void EINT_1(void)__irq
{
}

void EINT_2(void)__irq
{
}

void EINT_3(void)__irq
{
}

void TIMER0_MATCH(void)__irq
{
	leer_pulsadores();

	if(ESTADO == Rastrear)
	{
/*************** ALGORITMO DE RASTREO, CONTROL y RECONOCIMIENTO ***************/
		float w_ctrl = 0, vl = 0;

		leer_sensores();

		get_vel_encoders();
		get_pos_vel_robot();
		//almacenar datos absolutos del movimiento del robot y reconocer circuito

		w_ctrl = get_W(); //realimentación con encoders para cambio de línea con trayectorias circulares
		vl = get_VL();

		set_motors(vl, w_ctrl);
/******************************************************************************/

		if(button_stop == ON) // Condición de parada
		{
			MOTOR_SPEED_set(MOTOR_IZQ, velocidad, FREE);
			MOTOR_SPEED_set(MOTOR_DER, velocidad, FREE);
			PWMLER = (1<<0)|(1<<MI_PWM)|(1<<MD_PWM);
	
			LED_set(LED_1, OFF);
			LED_set(LED_2, OFF);

			ESTADO = Parado;
			cont_start = 0;
		}
	}
	else
	{
		switch (ESTADO)
		{
			case Seleccion_velocidad:
				selec_velocidad();
		
				if(button_b1 == ON)
					ESTADO = Parado;
	
				break;
	
			case Parado:
				if(button_load == ON)
				{
					inicializar_umbrales(1023, 500, 0);
					ESTADO = Calibrar_sensores;
		
					LED_set(LED_1, ON);
					LED_set(LED_2, OFF);
				}
				else
				{
					if(button_stop == ON && cont_start < 0.5)
						cont_start += PER_PERIOD;
					else if(button_stop == ON && cont_start >= 0.5)
						LED_set(LED_2, ON);
					else if(button_stop == OFF && cont_start >= 0.5)
					{
						LED_set(LED_2, OFF);
						
						integral = 0;
						proportional = 0;
						reset_encoders();
				
						if(seg_espera == 0)
							ESTADO = Rastrear;
						else
						{
							cont_espera = 0;
							ESTADO = Espera;
						}
					}
					else
						cont_start = 0;
		
					if(cont_start < 0.5)
					{
						if(cont_ready < 0.25)
							LED_set(LED_2, OFF);
						else if(cont_ready < 0.5)
							LED_set(LED_2, ON);
						else
						{
							LED_set(LED_2, OFF);
							cont_ready = 0;
						}
			
						cont_ready += PER_PERIOD;
					}
				}
	
				break;
	
			case Calibrar_sensores:
				leer_sensores();
				calibracion_estatica();
		
				if(button_b1 == ON)
				{
					ESTADO = Parado;
					cont_start = 0;
				}
	
				break;
	
			case Espera:
				cont_espera += PER_PERIOD;
				if(cont_espera >= seg_espera)
					ESTADO = Rastrear;
	
				break;
		}
	}

	comprobar_bateria();

	T0IR |= 1<<1;		// Activar flag de interrupción del Match0.1

	VICVectAddr = 0;	 
}

void TIMER1_MATCH(void)__irq
{
}

void TIMER0_CAPTURE(void)__irq
{
}

void TIMER1_CAPTURE(void)__irq
{
	if(T1IR&(1<<4))
	{
		if(T1CR0 < captura_izq)
			enc_izq = 2^32 + T1CR0 - captura_izq;
		else
			enc_izq = T1CR0 - captura_izq;

		captura_izq = T1CR0;
		cont_enc_izq++;

		T1IR |= 1<<4;		// Activar flag de interrupción del Capture1.0
	}
	else if(T1IR&(1<<5))
	{
		if(T1CR1 < captura_der)
			enc_der = 2^32 + T1CR1 - captura_der;
		else
			enc_der = T1CR1 - captura_der;

		captura_der = T1CR1;
		cont_enc_der++;

		T1IR |= 1<<5;		// Activar flag de interrupción del Capture1.1
	}

	VICVectAddr = 0;	 
}


#endif
