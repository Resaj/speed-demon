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
		leer_sensores();

//		get_vel_encoders();
//		get_pos_vel_robot();
		//almacenar datos absolutos del movimiento del robot y reconocer circuito

		w = get_W(); //realimentación con encoders para cambio de línea con trayectorias circulares
		v = get_VL();

		if(ESTADO_RASTREO == Rastreo_normal && ADCdist > 140)
		{
			cont_dist++;
			if(cont_dist >= 10)
			{
				ESTADO_RASTREO = Rastreo_cambio_lin;
				cont_cambio_lin = 0;
			}
		}
		else if(ESTADO_RASTREO == Rastreo_cambio_lin && cont_cambio_lin > 0.1)
			ESTADO_RASTREO = Rastreo_avanzar;
		else if(ESTADO_RASTREO == Rastreo_avanzar && negros != 0)
		{
			ESTADO_RASTREO = Rastreo_normal;
			cont_dist = 0;
	
			if(proportional < 0 && lin == DER && giro_ant == DER)
				lin = IZQ;
			else if(proportional > 0 && lin == IZQ && giro_ant == IZQ)
				lin = DER;
		}
		else
			cont_dist = 0;

		set_motors(v, w);
/******************************************************************************/

		actualizar();

		if(button_stop == ON) // Condición de parada
		{
			MOTOR_SPEED_set(MOTOR_IZQ, 30, FREE);
			MOTOR_SPEED_set(MOTOR_DER, 30, FREE);
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
						//reset_encoders();
				
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

//	comprobar_bateria();

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

void UART0(void)__irq
{
	switch(U0IIR & 0x0E)
	{
		case 0x04:	// Recepción
			uart0_dato = U0RBR;

			break;
		case 0x02:	// Transmisión
    		if(*uart0_ptr_tx == 0)
			{
				if(uart0_msg == '1') // Quedan mensajes por enviar
				{
					uart0_ptr_tx = uart0_buf_tx[uart0_rd]; // Se busca el nuevo mensaje para enviar
					uart0_rd++;
					uart0_rd &= N_MSG_UART0-1;
					if(uart0_rd == uart0_wr)
						uart0_msg = '0';	// Se va a enviar el último mensaje del buffer
				   	U0THR = *uart0_ptr_tx;
					uart0_ptr_tx++;
				}
				else
					uart0_repose = '1';
			}
			else	// Se continúa mandando el mensaje
				U0THR = *uart0_ptr_tx++;
	}	
    
	VICVectAddr = 0;
}


#endif
