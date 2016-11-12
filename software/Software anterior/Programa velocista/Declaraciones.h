#include <stdbool.h>

#ifndef Declaraciones_h
#define Declaraciones_h

#define Fpclk 19.6608e6*3/4	// Frecuencia del cristal del LPC2138
#define S 8					// Número de sensores

// Estados para implementar la máquina de estados
#define Parado '0'
#define Calcular_umbral '1'
#define Ejecucion '2'
extern char ESTADO;

extern float Periodo;
extern float Per_pwm;

// Variables auxiliares 
extern float cont;
extern int i;
extern int t;
extern int v;
extern int flag;
extern float start;
extern float stop;
extern int encoder_izq;
extern int encoder_der;
extern float radio[20];
extern int cond;

// Variables para el control del rastreo
extern unsigned int ADCresult[S];
extern unsigned int LINEAresult[S];
extern unsigned long r, ch;

extern unsigned int umbral[S];
extern unsigned int blanco[S];	
extern unsigned int negro[S];

// Variables para el control de velocidad
extern float v_lenta;
extern float v_rapida;
extern int frenar;
extern float velocidad;

#endif


// Configuraciones
void config_ADC0(void);
void config_TIMER0(void);
void config_TIMER1(void);
void config_MOTORES(void);
void config_leds(void);

// Interrupciones
void ADC0(void)__irq;
void TIMER0(void)__irq;
void TIMER1(void)__irq;

// Funciones de control
void calcular_umbral(void);
void digitalizar(void);
void calcular_radio(void);
void asignar_velocidad(void);
