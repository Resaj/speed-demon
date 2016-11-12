#include <lpc213x.h>
#include <stdbool.h>
#include "Declaraciones.h"
#include "PID.h"

/************************ VARIABLES ************************/
char ESTADO = Parado;

float Periodo = 5e-3;			// Periodo de ejecución del timer en modo match
float Per_pwm = 1e-3;			// Periodo de la PWM0

// Variables auxiliares 
float cont = 0;
int i = 0;
int t = 0;
int v = 0;
int flag = 1;
float start = 0;
float stop = 0;
int encoder_izq = 0;
int encoder_der = 0;
float radio[20];
int cond = 0;

// Variables para el control del rastreo
unsigned int ADCresult[S];		// Array con los datos del ADC ordenados
unsigned int LINEAresult[S];	// Array que contiene los valores digitalizados de ADCresult
unsigned long r, ch;			// Variables para guardar los datos del ADC

unsigned int umbral[S];			// Umbral para diferenciar 0 y 1 en la lectura del sensor
unsigned int blanco[S];	
unsigned int negro[S];

// Variables para el control de velocidad
float v_lenta = 0.5;			// Velocidad mínima en curva. 1 max
float v_rapida = 0.65;				// Velocidad máxima en recta. 1 max
int frenar = 0;
float velocidad = 0.5;			// Velocidad de rastreo

//Variables PID
int sensors_average = 0;
int sensors_sum = 0;
float position = 0;
float proportional = 0;
float integral = 0;
float derivative = 0;
float last_proportional = 0;
float control_value = 0;
int max_difference = (S-1)*1000/2;
float Kp = 3;	//1.5
float Ki = 0;
float Kd = 25;	//20


/************************** FUNCIÓN MAIN **************************/
int main (void)
{
	// Actualizar umbrales
	for(i=0; i<S; i++)
		umbral[i] = 600;

	// Iniciar vector de radios
	for(i=0; i<20; i++)
		radio[i] = -1;

	// Encender leds
	IO1PIN |= 1<<25;
	IO1PIN |= 1<<30;

	// Motores siempre hacia delante o frenados
	IO1PIN |= (1<<21)|(1<<27);
	IO1PIN &= ~((1<<22)|(1<<28));

	// Configuraciones previas
	config_leds();
	config_MOTORES();
	config_TIMER1();
	config_ADC0();
	config_TIMER0();

	while(1);
}
