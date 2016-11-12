#ifndef CONTROL_C
#define CONTROL_C

#include <lpc213x.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "LPClib.h"
#include "target.h"
#include "declaraciones.h"
#include "font.h"
#include "s1d15g00.h"
#include "motors.h"


/********************** FUNCIONES DE CONTROL **********************/
void selec_velocidad(void)
{
	if(button_load == ON && flag == '1')
	{
		switch ((int)velocidad)
		{
			case 40:
				LED_set(LED_1, ON);
				LED_set(LED_2, OFF);

				velocidad = 60;

				break;
			case 60:
				LED_set(LED_1, OFF);
				LED_set(LED_2, ON);
	
				velocidad = 80;

				break;
			case 80:
				LED_set(LED_1, ON);
				LED_set(LED_2, ON);
	
				velocidad = 100;

				break;
			case 100:
				LED_set(LED_1, OFF);
				LED_set(LED_2, OFF);
	
				velocidad = 40;
		}
	}

	if(button_load == ON)
		flag = '0';
	else // button_load == OFF
		flag = '1';
}

void leer_pulsadores(void)
{
	if(BUTTON_status(BUTTON_LOAD) == ON)
	{
		if(cont_load >= 0.05)
			button_load = ON;
		else // cont_load < 0.1
			cont_load += PER_PERIOD;
	}
	else
	{
		button_load = OFF;
		cont_load = 0;
	}

	if(BUTTON_status(BUTTON_B1) == ON)
	{
		if(cont_b1 >= 0.05)
			button_b1 = ON;
		else // cont_b1 < 0.1
			cont_b1 += PER_PERIOD;
	}
	else
	{
		button_b1 = OFF;
		cont_b1 = 0;
	}

	if(BUTTON_status(BUTTON_STOP) == ON)
	{
		if(cont_stop >= 0.05)
			button_stop = ON;
		else // cont_stop < 0.1
			cont_stop += PER_PERIOD;
	}
	else
	{
		button_stop = OFF;
		cont_stop = 0;
	}
}

void leer_sensores(void)
{
	ADCresult[0] = LINE_SENSOR_read(LINE_1);
	ADCresult[1] = LINE_SENSOR_read(LINE_2);
	ADCresult[2] = LINE_SENSOR_read(LINE_3);
	ADCresult[3] = LINE_SENSOR_read(LINE_4);
	ADCresult[4] = LINE_SENSOR_read(LINE_5);
	ADCresult[5] = LINE_SENSOR_read(LINE_6);
	ADCresult[6] = LINE_SENSOR_read(LINE_7);
	ADCresult[7] = LINE_SENSOR_read(LINE_8);
}

void inicializar_umbrales(unsigned int bl, unsigned int umb, unsigned int neg)
{
	int i = 0;

	for(i=0; i<S; i++)
	{
		blanco[i] = bl;
		umbral[i] = umb;
		negro[i] = neg;
	}
}

void calibracion_estatica(void)
{
	int var = 0, i = 0;

	for(i=0; i<S; i++)
	{
		if(ADCresult[i] < blanco[i])
			blanco[i] = ADCresult[i];
		if(ADCresult[i] > negro[i])
			negro[i] = ADCresult[i];

		if((negro[i]-blanco[i]) > 200)
		{
			var++;
			umbral[i] = (negro[i]+blanco[i])/2;
		}
	}

	if(var == S)
	{
		LED_set(LED_1, OFF);
		LED_set(LED_2, ON);
	}
}

void reset_encoders(void)
{
	captura_izq = 0;
	captura_der = 0;

	enc_izq = 0;
	enc_der = 0;

	cont_enc_izq = 0;
	cont_enc_der = 0;
}

void get_vel_encoders(void)
{
	float F = Fpclk;

	v_izq_real = ENCODER_speed(DIAMETER, EDGE_NUM, enc_izq/F);
	v_der_real = ENCODER_speed(DIAMETER, EDGE_NUM, enc_der/F);

	//filtrar tiempos de encoder
}

void get_pos_vel_robot(void)
{
	vl_real = (v_izq_real + v_der_real)/2;
	w_real = (v_der_real - v_izq_real)/((float)DIST_WHE/1000);
	//determinar posición o distancia recorrida (respecto a un punto de referencia relativo, no absoluto) y orientación actuales del robot

	//más adelante obtener velocidad angular con el giróscopo para evitar errores en derrapes
}

void representar_sensores(char modo)
{
	char cadena[30];
	int i = 0;

	if(modo == DIGITAL)
	{
		for(i = 0; i < S; i++)
		{
			if(ADCresult[i] > umbral[i])
				LCDSetRect((120-(i*10)), 10, (125-(i*10)), 15, 1, RED);
			else
				LCDSetRect((120-(i*10)), 10, (125-(i*10)), 15, 1, WHITE);
		}
	}
	else
	{
		for(i = 0; i < S; i++)
		{
			sprintf(cadena, "%4d", ADCresult[i]);   
			LCDPutStr(cadena, (120-(i*10)), 15, SMALL, WHITE, BLACK);
		}
	}
}

void representar_estado(void)
{
	char cadena[30];

	sprintf(cadena, "EST %c", ESTADO);   
	LCDPutStr(cadena, 86, 60, SMALL, WHITE, BLACK);
}

void comprobar_bateria(void)
{
	// Comprobación del nivel de batería
	if(cont_bat >= BAT_PERIOD)
	{
		cont_bat = 0;
		GPIO_set(MUX, '1');
		blinky = LIPO_TENSION_read(BAT_ADC, 7.6/8.4*2.88);
	}

	cont_bat += PER_PERIOD;

	// Led parpadeando con batería baja
	if(blinky == '1')
	{
		cont_blinky += PER_PERIOD;
		if(cont_blinky < 0.4)
			LED_set(LED_1, OFF);
		else if(cont_blinky >= 0.4)
		{
			LED_set(LED_1, ON);

			if(cont_blinky >= 0.5)
				cont_blinky = 0;
		}
	} 
}


#endif
