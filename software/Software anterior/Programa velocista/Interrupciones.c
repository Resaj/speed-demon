#include <lpc213x.h>
#include <stdlib.h>
#include "Declaraciones.h"
#include "PID.h"

/************************ INTERRUPCIONES ************************/
// Función que realiza la lectura de los sensores de línea
void ADC0(void)__irq
{
	r = AD0DR;
	ch = (r>>24)&0x07;
	ADCresult[ch] = (r>>6)&0x03FF;

	VICVectAddr = 0;
}

// Interrupción para el capture1.2 y capture1.3
void TIMER1(void)__irq
{
	if(T1IR&(1<<6))
	{
//		if(t == 1)
//		{
//			IOCLR1 = 1<<25;
//			t = 0;
//		}
//		else
//		{
//			IOSET1 = 1<<25;
//			t = 1;
//		}

		if(ESTADO == Ejecucion)
			encoder_izq++;

		T1IR |= 1<<6;		// Activar flag de interrupción del Capture1.2
	}
	else if(T1IR&(1<<7))
	{
//		if(v == 1)
//		{
//			IOCLR1 = 1<<30;
//			v = 0;
//		}
//		else
//		{
//			IOSET1 = 1<<30;
//			v = 1;
//		}

		if(ESTADO == Ejecucion)
			encoder_der++;

		T1IR |= 1<<7;		// Activar flag de interrupción del Capture1.3
	}

	VICVectAddr = 0;	 
}


// Interrupción IRQ para el TIMER0
void TIMER0(void)__irq
{
	if(ESTADO == Parado)
	{
		if((IO0PIN&(1<<14)) == 0)
		{
			for(i=0; i<S; i++)
			{
				blanco[i] = 1023;
				negro[i] = 0;
			}
			ESTADO = Calcular_umbral;
			IO1PIN &= ~(1<<25);
			IO1PIN |= 1<<30;
		}
		else
		{
			if((IO0PIN&(1<<15)) == 0 && start < 0.5)
				start += Periodo;
			else if((IO0PIN&(1<<15)) == 0 && start >= 0.5)
			{
				IOSET1 = 1<<25;
				IOSET1 = 1<<30;
			}
			else if((IO0PIN&(1<<15)) != 0 && start >= 0.5)
			{
				integral = 0;
				last_proportional = 0;

				IO1PIN &= ~(1<<16);
				velocidad = v_lenta;
				ESTADO = Ejecucion;
				IO1PIN &= ~(1<<25);
				IO1PIN &= ~(1<<30);
				stop = 0;
			}
			else
				start = 0;

			if(start < 0.5)
			{
				if(cont > 0.2)
				{
					if(t == 1)
					{
						IOCLR1 = 1<<25;
						IOCLR1 = 1<<30;
						t = 0;
					}
					else
					{
						IOSET1 = 1<<25;
						IOSET1 = 1<<30;
						t = 1;
					}
					cont = 0;
				}
	
				cont += Periodo;
			}
		}
	}
	else if(ESTADO == Calcular_umbral)
	{
		if((IO0PIN&(1<<16)) == 0)
			ESTADO = Parado;
		else
			calcular_umbral();
	}
	else if(ESTADO == Ejecucion)
	{
		if((IO0PIN&(1<<16)) == 0)
			stop += Periodo;
		else
			stop = 0;

		if(stop >= 0.1)
		{
			PWMMR2 = 0;
			PWMMR6 = 0;
			PWMLER = (1<<0)|(1<<2)|(1<<6);
			IO1PIN |= 1<<16;
			ESTADO = Parado;
			IO1PIN |= 1<<25;
			IO1PIN |= 1<<30;
			start = 0;
		}
		else
		{
			calcular_radio();
			asignar_velocidad();

			digitalizar();
			loop_PID();
		}
	}
	
	T0IR |= (1<<2);
	VICVectAddr = 0;
}
