#include <lpc213x.h>
#include <stdbool.h>
#include <stdio.h>
#include "LPClib.h"
#include "declaraciones.h"
#include "target.h"
#include "s1d15g00.h"
#include "motors.h"


/************************ VARIABLES ************************/
// Variables auxiliares 
char ESTADO = Seleccion_velocidad, ESTADO_RASTREO = Rastreo_normal;
char cadena[200];			// Variable auxiliar para guardar el texto a imprimir en el LCD
float cont_load = 0, cont_b1 = 0, cont_stop = 0,  cont_start = 0, cont_ready = 0, cont_espera = 0, cont_bat = BAT_PERIOD, cont_blinky = 0;
char flag = ON, button_load = OFF, button_b1 = OFF, button_stop = OFF;
float cont_sel = 0;
float cont_cambio_lin = 0;
int cont_dist = 0;
unsigned long captura_izq = 0, captura_der = 0;
unsigned long enc_izq = 0, enc_der = 0;
unsigned int cont_enc_izq = 0, cont_enc_der = 0;
char blinky = OFF;
char uart0_dato = '0';

unsigned int seg_espera = 0;
char color_suelo = BLANCO;

// Variables para el control del rastreo
unsigned int ADCresult[S];		// Array con los datos del ADC ordenados
unsigned int ADCdist;

unsigned int umbral[S];			// Umbral para diferenciar blanco y negro en la lectura del sensor
unsigned int blanco[S];	
unsigned int negro[S];

char giro = '0', giro_ant;		// Variable auxiliar para no perder la línea. Se inicia a un valor distinto a IZQ y DER
int negros = 0;					// Número de sensores en negro
char lin = '3';					// Línea a seguir

// Variables para el control de velocidad
float v_rapida = 60;			// Control de velocidad de rastreo normal. 1 max
//float velocidad = 60;			// Velocidad de rastreo. Entre 0 y 100
float w = 0, v = 0;
//float v_izq_real = 0, v_der_real = 0, vl_real = 0, w_real = 0;
char mode_izq = FORWARD, mode_der = FORWARD;

//Variables del PID de velocidad angular
float proportional = 0;
float integral = 0;
float integral_max = 1;
float w_ctrl_max = 3;
float Kp = 1;
float Ki = 0;
float Kd = 70;


// Sensores
struct analog LINE_1	= {ADC_1,ADC_CH6};	// Número y canal de ADC para el sensor de línea 1
struct analog LINE_2	= {ADC_1,ADC_CH7};	// Número y canal de ADC para el sensor de línea 2
struct analog LINE_3	= {ADC_0,ADC_CH4};	// Número y canal de ADC para el sensor de línea 3
struct analog LINE_4	= {ADC_0,ADC_CH5};	// Número y canal de ADC para el sensor de línea 4
struct analog LINE_5	= {ADC_0,ADC_CH0};	// Número y canal de ADC para el sensor de línea 5
struct analog LINE_6	= {ADC_0,ADC_CH1};	// Número y canal de ADC para el sensor de línea 6
struct analog LINE_7	= {ADC_0,ADC_CH2};	// Número y canal de ADC para el sensor de línea 7
struct analog LINE_8	= {ADC_0,ADC_CH3};	// Número y canal de ADC para el sensor de línea 8

struct analog DIST		= {ADC_0,ADC_CH6};	// Sensor de distancia frontal

// Batería
struct analog BAT_ADC	= {ADC_1,ADC_CH3};	// Número y canal de ADC para la batería
struct data MUX			= {0,16};			// Puerto y pin del multiplexor

// Pulsadores
struct data BUTTON_LOAD	= {0,14};	// Puerto y pin del pulsador de load
struct data BUTTON_B1	= {1,18};	// Puerto y pin del pulsador B1
struct data BUTTON_STOP	= {1,21};	// Puerto y pin del pulsador de start/stop

// Leds
struct data LED_1	= {1,16};		// Puerto y pin del led 1
struct data LED_2	= {1,17};		// Puerto y pin del led 2

// Motores
struct data MI_pos	= {1,26};		// Control de Puente en H +, rama izquierda
struct data MI_neg	= {1,25};		// Control de Puente en H -, rama izquierda
struct data MD_pos	= {1,27};		// Control de Puente en H +, rama derecha
struct data MD_neg	= {1,28};		// Control de Puente en H -, rama derecha

struct motor MOTOR_IZQ	= {MI_PWM,{1,26},{1,25}};
struct motor MOTOR_DER	= {MD_PWM,{1,27},{1,28}};

// Encoders
struct data ENC_IZQ	= {0,10};		// Puerto y pin del encoder izquierdo
struct data ENC_DER	= {0,11};		// Puerto y pin del encoder derecho


/************************ FUNCIONES DE CONFIGURACIÓN ************************/
void config_SPEED_DEMON(void)
{
	config_ADC();
	config_BUTTONS();
//	config_LCD();
	config_LEDS();
	config_MOTORS();
	config_UART();
//	config_CAPTURE();
	config_PERIODIC_CONTROL();
}

void config_LINE_SENSORS(void)
{
	ADC_CH_config(LINE_1);
	ADC_CH_config(LINE_2);
	ADC_CH_config(LINE_3);
	ADC_CH_config(LINE_4);
	ADC_CH_config(LINE_5);
	ADC_CH_config(LINE_6);
	ADC_CH_config(LINE_7);
	ADC_CH_config(LINE_8);
}

void config_DIST_SENSORS(void)
{
	ADC_CH_config(DIST);
}

void config_BATTERY_ADC(void)
{
	ADC_CH_config(BAT_ADC);
	GPIO_config(MUX, OUT);
}

void config_ADC(void)
{
	config_LINE_SENSORS();
	config_DIST_SENSORS();
	config_BATTERY_ADC();

	ADC_enable(ADC_0);
	ADC_enable(ADC_1);
}

void config_BUTTONS(void)
{
	GPIO_config(BUTTON_LOAD, IN);
	GPIO_config(BUTTON_B1, IN);
	GPIO_config(BUTTON_STOP, IN);
}

void config_LCD(void)
{
	LCD_BL_DIR();			// BL = Output
	LCD_CS_DIR();			// CS = Output
	LCD_SCLK_DIR();			// SCLK = Output
	LCD_SDATA_DIR();		// SDATA = Output
	LCD_RESET_DIR();		// RESET = Output
	LCD_SCLK_LOW();			// Standby SCLK
	LCD_CS_HIGH();			// Disable CS
	LCD_SDATA_HIGH();		// Standby SDATA
	LCD_BL_HIGH();			// Black Light ON = 100%
	
	InitLcd(); 				// Initial LCD
	LCDClearScreen();		// Limpiar pantalla
}

void config_LEDS(void)
{
	GPIO_config(LED_1, OUT);
	GPIO_config(LED_2, OUT);
}

void config_MOTORS(void)
{
	MOTOR_config(MOTOR_IZQ);
	MOTOR_config(MOTOR_DER);
											 
	PWM_enable(PWM_PERIOD);
}

void config_UART(void)
{
	UART0_config(115200, UART_PRIO);
}

void config_CAPTURE(void)
{
	CAPTURE_CHANNEL_config(ENC_IZQ_CH, ENC_IZQ, DOUBLE_EDGE);
	CAPTURE_CHANNEL_config(ENC_DER_CH, ENC_DER, DOUBLE_EDGE);

	TIMER_config(CAPTURE_TIMER, CAPTURE_MODE, CAPTURE_PRIO);
}

void config_PERIODIC_CONTROL(void)
{
	MATCH_CHANNEL_config(PERIODIC_CH, PER_PERIOD);
	TIMER_config(MATCH_TIMER, MATCH_MODE, MATCH_PRIO);
}


/************************** FUNCIÓN MAIN **************************/
int main (void)
{
	inicializar_umbrales(50, 400, 800);

	config_SPEED_DEMON();

	LED_set(LED_1, ON);
	LED_set(LED_2, ON);

	while(1) // Bucle infinito para la ejecución del programa
	{
		if(uart0_dato == 'l')
		{
			sprintf(cadena, "%4u %4u %4u %4u %4u %4u %4u %4u\n\r", ADCresult[0], ADCresult[1], ADCresult[2], ADCresult[3],
				ADCresult[4], ADCresult[5], ADCresult[6], ADCresult[7]);
			UART0_send(cadena);
		}
		else if(uart0_dato == 'u')
		{
			sprintf(cadena, "%4u %4u %4u %4u %4u %4u\n\r%4u %4u %4u %4u %4u %4u\n\r%4u %4u %4u %4u %4u %4u\n\r", blanco[0],
				blanco[1], blanco[2], blanco[3], blanco[4], blanco[5], umbral[0], umbral[1], umbral[2], umbral[3], umbral[4],
				umbral[5], negro[0], negro[1], negro[2], negro[3], negro[4], negro[5]);
			UART0_send(cadena);
		}
		else if(uart0_dato == 'd')
		{
			sprintf(cadena, "%d\n\r", ADCdist);
			UART0_send(cadena);
		}
		else if(uart0_dato == 'm')
		{
			sprintf(cadena, "%4f\t%4f\n\r", w, v);
			UART0_send(cadena);
		}
	}
}
