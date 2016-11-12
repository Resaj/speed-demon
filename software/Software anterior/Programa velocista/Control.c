#include <lpc213x.h>
#include <stdlib.h>
#include "Declaraciones.h"

/********************** FUNCIONES DE CONTROL **********************/
void calcular_umbral(void)
{
	cont = 0;	
	for(i=0; i<S; i++)
	{
		if(ADCresult[i] < blanco[i])
			blanco[i] = ADCresult[i];
		if(ADCresult[i] > negro[i])
			negro[i] = ADCresult[i];

		if((negro[i]-blanco[i]) > 200)
		{
			cont++;
			umbral[i] = (negro[i]+blanco[i])/2;
		}
	}	

	if(cont == S)
	{
		IO1PIN |= 1<<25;
		IO1PIN &= ~(1<<30);
	}
}

void digitalizar(void)
{
	for(i=0; i<S; i++)
	{
		if(ADCresult[i] > umbral[i])
			LINEAresult[i] = 1;
		else
			LINEAresult[i] = 0;
	}
}

void calcular_radio(void)
{

	if(encoder_izq > 12 || encoder_der > 12)
	{
		if(encoder_izq == encoder_der)
			encoder_der--;

		for(i=19; i>0; i--)
			radio[i] = radio[i-1];

		if(encoder_izq > encoder_der)
		{
			cont += encoder_izq;
			radio[0] = -(7.25 + 14.5*encoder_der/(encoder_izq - encoder_der));
		}
		else // encoder_der > 12
		{
			cont += encoder_der;
			radio[0] = 7.25 + 14.5*encoder_izq/(encoder_der - encoder_izq);
		}

		encoder_izq = 0;
		encoder_der = 0;
	}
}

void asignar_velocidad(void)
{
	if(cond == 0 && radio[0] < 40)
		cond = 1;

	if(radio[0] > 40 && radio[1] < 40 && cond == 1)
	{
		cond = 0;
		cont = 0;
		velocidad = v_rapida;
	}
	else if(cont >= 30)
	{
		frenar = 1;
	}
	if(cont >= 33)
	{
		velocidad = v_lenta;
		frenar = 0;
	}


//	if(control_value > -800 &&	control_value < 800)
//		cont++;
//	if(0.1 > Periodo * cont)
//	{
//		Kp = 1;
//		Kd = 10;
//		velocidad = v_rapida;
//	}
//	else if(0.5 > Periodo * cont)
//	{
//		//Kp = 1.5;
//		//Kd = 20;
//		velocidad = v_lenta;
//	}
}
