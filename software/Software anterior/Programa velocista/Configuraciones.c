#include <lpc213x.h>
#include "Declaraciones.h"

/************************ CONFIGURACIONES ************************/
// Configuración del ADC0 para los sensores de línea
void config_ADC0 (void)
{
	PINSEL0 |= (1<<8)|(1<<9)|(1<<10)|(1<<11);		// Habilitar ADC0[6,7]
	PINSEL1 |= (1<<18)|(1<<20)|(1<<22)|(1<<24)|(1<<26)|(1<<28);	// Habilitar ADC0[0..5]

	VICVectAddr2 = (unsigned long) ADC0;
	VICVectCntl2 = (0x20|18);
	VICIntEnable |= (1<<18);

	AD0CR = 0x2103FF;	// Activación de la conversión en modo burst para los 8 canales de ADC0
}

// Configuración del Match0.2 del TIMER0
void config_TIMER0 (void)
{
	T0TCR = 0x02;
	T0PR = 0;
	T0MCR = (1<<6)|(1<<7);
	T0MR2 = (unsigned long)(Periodo*Fpclk);
	VICVectAddr1 = (unsigned long) TIMER0;
	VICVectCntl1 = (0x20|4);
	T0TCR = 0x01;
	VICIntEnable |= 1<<4;
}

// Configuración del Capture1.2 y Capture1.3
void config_TIMER1(void)
{
	PINSEL1 |= (1<<2)|(1<<4);
	T1TCR = 0x02;
	T1CTCR = 0;
	T1PR = 0;
	T1TCR = 0x01;
 	T1CCR = (7<<6)|(7<<9);
	VICVectCntl0 = (0x20|5);
	VICVectAddr0 = (unsigned long) TIMER1;
	VICIntEnable |= (1<<5);
}

// Configuración de las salidas de PWM para los motores
void config_MOTORES (void)
{
	IO1PIN &= ~(1<<16);
	PINSEL0 |= (1<<15)|(1<<19);					// PWM2 y PWM6
	IO1DIR |= (1<<21)|(1<<22)|(1<<27)|(1<<28);	// Salidas para el sentido de los motores
	PWMTCR = 0x02;								// Reset TIMER y PREESCALER										   
	PWMPR = 0;									// El PRESCALER no modifica la frecuencia
	PWMMR0 = Per_pwm * Fpclk;					// Periodo PWM									   
	PWMLER = (1<<0)|(1<<2)|(1<<6);				// Habilitar MATCH0, MATCH2 y MATCH6
	PWMMCR = 0x02;								// Reset TIMER COUNTER REGISTER ON MATCH0							   
	PWMPCR = (1<<10)|(1<<14);					// Habilitar PWM2 y PWM6 en flanco
	PWMTCR = (1<<0)|(1<<3);						// Habilitar PWM y comenzar la cuenta
}

// Configuración de los leds
void config_leds(void)
{
	IO1DIR |= (1<<25)|(1<<30);
}
