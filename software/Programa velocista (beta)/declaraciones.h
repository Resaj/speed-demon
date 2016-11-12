#ifndef DECLARACIONES_H
#define DECLARACIONES_H

#include <stdbool.h>
#include "LPClib.h"

// Estados para implementar la m�quina de estados
#define Parado 				'0'
#define Seleccion_velocidad	'1'
#define Calibrar_sensores	'2'
#define Espera				'3'
#define Rastrear			'4'

#define S 8					// N�mero de sensores

#define BLANCO		'0'
#define NEGRO		'1'

#define DIGITAL		'0'
#define ANALOGICO	'1'


// Variables auxiliares
extern char ESTADO;
extern float cont_load, cont_b1, cont_stop, cont_start, cont_ready, cont_espera, cont_bat, cont_blinky;
extern char flag, button_load, button_b1, button_stop;
extern unsigned long captura_izq, captura_der;
extern unsigned long enc_izq, enc_der;
extern unsigned int cont_enc_izq, cont_enc_der;
extern char blinky;

extern unsigned int seg_espera;
extern char color_suelo;

// Variables para el control del rastreo
extern unsigned int ADCresult[S];
extern unsigned int umbral[S];
extern unsigned int blanco[S];	
extern unsigned int negro[S];


// Prioridades de interrupciones
#define CAPTURE_PRIO	0		// Prioridad del timer en modo capture para los encoders
#define MATCH_PRIO		1		// Prioridad del timer en modo match para el control peri�dico

// Sensores
extern struct analog LINE_1;	// N�mero y canal de ADC para el sensor de l�nea 1
extern struct analog LINE_2;	// N�mero y canal de ADC para el sensor de l�nea 2
extern struct analog LINE_3;	// N�mero y canal de ADC para el sensor de l�nea 3
extern struct analog LINE_4;	// N�mero y canal de ADC para el sensor de l�nea 4
extern struct analog LINE_5;	// N�mero y canal de ADC para el sensor de l�nea 5
extern struct analog LINE_6;	// N�mero y canal de ADC para el sensor de l�nea 6
extern struct analog LINE_7;	// N�mero y canal de ADC para el sensor de l�nea 7
extern struct analog LINE_8;	// N�mero y canal de ADC para el sensor de l�nea 8

// Bater�a
extern struct analog BAT_ADC;	// N�mero y canal de ADC para la bater�a
extern struct data MUX;			// Puerto y pin del multiplexor

// Pulsadores
extern struct data BUTTON_LOAD;	// Puerto y pin del pulsador de load
extern struct data BUTTON_B1;	// Puerto y pin del pulsador B1
extern struct data BUTTON_STOP;	// Puerto y pin del pulsador de start/stop

// Leds
extern struct data LED_1;		// Puerto y pin del led 1
extern struct data LED_2;		// Puerto y pin del led 2

// Motores
extern struct data MI_pos;		// Control de Puente en H +, rama izquierda
extern struct data MI_neg;		// Control de Puente en H -, rama izquierda
extern struct data MD_pos;		// Control de Puente en H +, rama derecha
extern struct data MD_neg;		// Control de Puente en H -, rama derecha

#define MI_PWM			PWM4	// Canal de PWM de los motores izquierdos
#define MD_PWM			PWM6	// Canal de PWM de los motores derechos

extern struct motor MOTOR_IZQ;
extern struct motor MOTOR_DER;

// Timers
#define MATCH_TIMER		TIMER0		// Timer a habilitar como MATCH
#define PERIODIC_CH		MATCH0_1	// Canal de MATCH de la interrupci�n peri�dica

#define CAPTURE_TIMER	TIMER1		// Timer a habilitar como CAPTURE
extern struct data ENC_IZQ;			// Puerto y pin del encoder izquierdo
#define ENC_IZQ_CH		CAPTURE1_0	// Canal de CAPTURE del encoder izquierdo
extern struct data ENC_DER;			// Puerto y pin del encoder derecho
#define ENC_DER_CH		CAPTURE1_1	// Canal de CAPTURE del encoder derecho

// Propiedades mec�nicas
#define DIAMETER		30			// Di�metro de las ruedas en mil�metros
#define EDGE_NUM		6			// Flancos de encoder en una vuelta de la rueda
#define DIST_WHE		143			// Separaci�n entre ruedas en mil�metros


// Funci�n general de configuraci�n
void config_SPEED_DEMON(void);

// Funciones de configuraci�n de los dispositivos
void config_LINE_SENSORS(void);
void config_BATTERY_ADC(void);
void config_ADC(void);
void config_BUTTONS(void);
void config_LCD(void);
void config_LEDS(void);
void config_MOTORS(void);
void config_CAPTURE(void);
void config_PERIODIC_CONTROL(void);

// Funciones de control
void selec_velocidad(void);
void leer_pulsadores(void);
void leer_sensores(void);
void inicializar_umbrales(unsigned int bl, unsigned int umb, unsigned int neg);
void calibracion_estatica(void);
void reset_encoders(void);
void get_vel_encoders(void);
void get_pos_vel_robot(void);
void representar_sensores(char modo);
void representar_estado(void);
void comprobar_bateria(void);


#endif
