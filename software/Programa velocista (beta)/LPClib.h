#ifndef LPCLIB_H
#define LPCLIB_H


#define ADC_0			0	// ADC 0
#define ADC_1			1	// ADC 1
#define ADC_CH0			0	// Canal 0 de ADC
#define ADC_CH1			1	// Canal 1 de ADC
#define ADC_CH2			2	// Canal 2 de ADC
#define ADC_CH3			3	// Canal 3 de ADC
#define ADC_CH4			4	// Canal 4 de ADC
#define ADC_CH5			5	// Canal 5 de ADC
#define ADC_CH6			6	// Canal 6 de ADC
#define ADC_CH7			7	// Canal 7 de ADC

struct analog
{
	int adc;
	int channel;
};

#define LEVEL		'0'	// Detección por nivel
#define EDGE		'1'	// Detección por flanco
#define DOWN		'0'	// Detección a nivel bajo o flanco de bajada
#define UP			'1'	// Detección a nivel alto o flanco de subida

void EINT_0(void)__irq;
void EINT_1(void)__irq;
void EINT_2(void)__irq;
void EINT_3(void)__irq;

#define pi 3.14159265

#define IN		'0'
#define OUT		'1'

#define PIN_OFF	'0'
#define PIN_ON	'1'

struct data
{
	int port;
	int pin;
};

#define ON '1'
#define OFF '0'	

#define FORWARD		'0'		// Sentido hacia delante
#define BACKWARD	'1'		// Sentido hacia atrás
#define LOCKED		'2'		// Freno
#define FREE		'3'		// Libre

struct motor
{
	int channel;						// Canal de PWM
	struct data bridge_h_pos_data_pin;	// Estructura con el número de puerto y pin de la primera conexión del puente en H
	struct data bridge_h_neg_data_pin;	// Estructura con el número de puerto y pin de la segunda conexión del puente en H
};

// Modos de funcionamiento
#define GPIO		'0'
#define	ADC			'1'
#define	DAC			'2'
#define	EINT		'3'
#define	CAPTURE		'4'
#define	MATCH		'5'
#define MATCH_2		'6'
#define	PWM			'7'
#define UART		'8'
#define I2C			'9'
#define	SPI			'A'

// Canales de PWM
#define PWM1	1
#define PWM2	2
#define PWM3	3
#define PWM4	4
#define PWM5	5
#define PWM6	6

#define SERVO_PERIOD	0.02
#define SERVO_MIN_POS	0.0006
#define SERVO_MAX_POS	0.0026

#define	TIMER0			'0'
#define TIMER1			'1'

#define MATCH_MODE		'0'
#define CAPTURE_MODE	'1'

#define MATCH0_0		'0'
#define MATCH0_1		'1'
#define MATCH0_2		'2'
#define MATCH0_3		'3'
#define MATCH1_0		'4'
#define MATCH1_1		'5'
#define MATCH1_2		'6'
#define MATCH1_3		'7'

#define CAPTURE0_0		'0'
#define CAPTURE0_1		'1'
#define CAPTURE0_2		'2'
#define CAPTURE0_3		'3'
#define CAPTURE1_0		'4'
#define CAPTURE1_1		'5'
#define CAPTURE1_2		'6'
#define CAPTURE1_3		'7'

#define RISING_EDGE		5
#define FALLING_EDGE	6
#define DOUBLE_EDGE		7

void TIMER0_MATCH(void)__irq;
void TIMER1_MATCH(void)__irq;
void TIMER0_CAPTURE(void)__irq;
void TIMER1_CAPTURE(void)__irq;


/* Función de configuración de los canales de ADC */
//		Parámetros:
//			data_adc --> número y canal de ADC a habilitar
void ADC_CH_config(struct analog data_adc);

/* Función de habilitación de ADC */
//		Parámetros:
//			adc --> número de ADC
void ADC_enable(int adc);

/* Función de lectura de ADC */
//		Parámetros:
//			data_adc --> número y canal de ADC que se va a leer
//		Devuelve el valor del ADC
unsigned int ADC_read(struct analog data_adc);

/* Función que devuelve el estado del pulsador indicado */
//		Parámetros:
//			button_data_pin --> estructura con el número de puerto y pin del pulsador a leer
//		Devuelve:
//			'1' --> el pulsador ha sido pulsado
//			'0' --> el pulsador no ha sido pulsado
char BUTTON_status(struct data button_data_pin);

/* Función de lectura y conversión del valor para los sensores GP2Y0A */
//		Parámetros:
//			data_adc --> número y canal de ADC a leer
//		Devuelve la distancia detectada en milímetros			
unsigned int GP2Y0A_read(struct analog data_adc);

/* Función de lectura y conversión del valor para los sensores GP2D120 */
//		Parámetros:
//			data_adc --> número y canal de ADC a leer
//		Devuelve la distancia detectada en milímetros			
unsigned int GP2D120_read(struct analog data_adc);

/* Función que lectura del valor para los sensores de distancia digitales */
//		Parámetros:
//			sensor_data_pin --> estructura con el número de puerto y pin del sensor a leer
//		Devuelve:
//			'1' --> el sensor detecta
//			'0' --> el sensor no detecta
char DISTANCE_digital_read(struct data sensor_data_pin);

/* Función de configuración de interrupciones externas */
//		Parámetros:
//			data_pin --> estructura con el número de puerto y pin de la interrupción externa
//			mode --> modo de detección de la interrupción ('1' por flanco, '0' por nivel)
//			polar --> nivel o flanco de detección de la interrupción ('1' flanco de subida o nivel alto, '0' flanco de bajada o nivel bajo)
//			priority --> prioridad de la interrupción externa a configurar
void EINT_config(struct data data_pin, char mode, char polar, unsigned int priority);

/* Función que calcula la distancia recorrida a partir de las mediciones del encoder */
//		Parámetros:
//			diameter --> diámetro de la rueda en milímetros
//			edge_num --> número de flancos del encoder en una vuelta de la rueda
//			cont --> número de flancos de encoder transcurridos en la medición
//		Devuelve la velocidad de la rueda en metros por segundo
float ENCODER_distance(float diameter, int edge_num, int cont);

/* Función que calcula la velocidad de la rueda a partir de las mediciones del encoder */
//		Parámetros:
//			diameter --> diámetro de la rueda en milímetros
//			edge_num --> número de flancos del encoder en una vuelta de la rueda
//			time --> tiempo transcurrido desde el flanco anterior en segundos
//		Devuelve la velocidad de la rueda en metros por segundo
float ENCODER_speed(float diameter, int edge_num, float time);

/* Función que configura una entrada/salida de propósito general */
//		Parámetros:
//			data_pin --> estructura con número de puerto y pin del GPIO a configurar
//			inout --> configuración del GPIO ('0' para entrada, '1' para salida)
void GPIO_config(struct data data_pin, char inout);

/* Función que modifica el valor de la salida de propósito general indicada */
//		Parámetros:
//			data_pin --> estructura con número de puerto y pin del GPIO a configurar
//			status --> nuevo estado del GPIO ('1' para activación, '0' para desactivación)
void GPIO_set(struct data data_pin, unsigned char status);

/* Función que lee el valor de la entrada de propósito general indicada */
//		Parámetros:
//			data_pin --> estructura con número de puerto y pin del GPIO a configurar
//		Devuelve:
//			'0' --> pin a nivel bajo
//			'1'	--> pin a nivel alto
char GPIO_read(struct data data_pin);

/* Función que modifica el valor de los leds */
//		Parámetros:
//			led_data_pin --> estructura con el número de puerto y pin del led a modificar
//			status --> nuevo estado del led ('1' para encender, '0' para apagar)
void LED_set(struct data led_data_pin, unsigned char status);

/* Función de lectura para los sensores de línea */
//		Parámetros:
//			data_adc --> número y canal de ADC a leer
//		Devuelve el valor de ADC			
unsigned int LINE_SENSOR_read(struct analog data_adc);

/* Función de lectura de tensión de las baterías lipo */
//		Parámetros:
//			data_adc --> número y canal de ADC a leer
//			v_min --> tensión mínima permitida, en voltios (entre 0 y 3v3)
//		Devuelve:
//			'1' --> nivel de batería bajo
//			'0' --> nivel de batería aceptable
char LIPO_TENSION_read(struct analog data_adc, float v_min);

/* Función de configuración de PWM */
//		Parámetros:
//			MOTOR --> estructura con los pines del puente en H y el canal de PWM
void MOTOR_config(struct motor MOTOR);

/* Función que asigna un valor a la velocidad de un motor */
//		Parámetros:
//			MOTOR --> estructura con los pines del puente en H y el canal de PWM
//			speed --> valor de activación (tanto para velocidad como para frenado) (entre 0 y 100)
//			mode --> modo de funcionamiento (FORWARD, BACKWARD, LOCKED, FREE)
void MOTOR_SPEED_set(struct motor MOTOR, float speed, char mode);

/* Función para la selección del modo de los pines */
//		Parámetros:
//			data_pin --> estructura con el número de puerto y pin
//			mode --> selección del modo
void PINSEL_config(struct data data_pin, char mode);

/* Función de configuración de los canales de PWM */
//		Parámetros:
//			channel --> canal de PWM que se va a habilitar
void PWM_CH_config(int channel);

/* Función de configuración y habilitación de PWM */
//		Parámetros:
//			period --> periodo de PWM
void PWM_enable(float period);

/* Función para modificar el valor de PWM */
//		Parámetros:
//			channel --> canal de PWM cuyo valor se va a modificar
//			value --> nuevo valor de PWM (entre 0 y 100)
void PWM_set(int channel, float value);

/* Función de configuración de PWM */
//		Parámetros:
//			channel --> canal de PWM
void SERVO_config(int channel);

/* Función que modifica el valor del servo */
//		Parámetros:
//			channel --> canal del servo
//			position --> nuevo valor de la posición del servo (entre 0 y 100)
void SERVO_set(int channel, float position);

/* Función de configuración de los canales del timer como interrupción periódica */
//		Parámetros:
//			channel --> canal del timer
//			period --> periodo de ejecución del timer
void MATCH_CHANNEL_config(char channel, float period);

/* Función de configuración de los canales del timer en modo captura */
//		Parámetros:
//			channel --> canal del timer
//			data_pin --> estructura con número de puerto y pin del timer
//			edge --> flanco de captura ('0' flanco de subida, '1' flanco de bajada, '2' doble flanco)
void CAPTURE_CHANNEL_config(char channel, struct data data_pin, char edge);

/* Función de configuración de timer */
//		Parámetros:
//			timer --> número de timer a configurar ('0' timer 0, '1' timer 1)
//			mode --> modo de configuración del timer ('0' match, '1' capture)
//			priority --> prioridad a asignar al timer
void TIMER_config(char timer, char mode, unsigned int priority);

/* Función para asignar las interrupciones */
//		Parámetros:
//			name --> nombre a asignar a la interrupción
//			priority --> prioridad de la interrupción
//			num --> número de interrupción
void VIC_assign(unsigned long name, unsigned int priority, unsigned int num);


#endif
