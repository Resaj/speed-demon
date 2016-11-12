#ifndef LPCLIB_C
#define LPCLIB_C

#include <lpc213x.h>
#include "LPClib.h"
#include "target.h"


void ADC_CH_config(struct analog data_adc)
{
	struct data data_pin;
	data_pin.port = 0;

	if(data_adc.adc == ADC_0)
	{
		switch(data_adc.channel)
		{
			case ADC_CH0:
				data_pin.pin = 27;
				PINSEL_config(data_pin, ADC);
				break;
			case ADC_CH1:
				data_pin.pin = 28;
				PINSEL_config(data_pin, ADC);
				break;
			case ADC_CH2:
				data_pin.pin = 29;
				PINSEL_config(data_pin, ADC);
				break;
			case ADC_CH3:
				data_pin.pin = 30;
				PINSEL_config(data_pin, ADC);
				break;
			case ADC_CH4:
				data_pin.pin = 25;
				PINSEL_config(data_pin, ADC);
				break;
			case ADC_CH5:
				data_pin.pin = 26;
				PINSEL_config(data_pin, ADC);
				break;
			case ADC_CH6:
				data_pin.pin = 4;
				PINSEL_config(data_pin, ADC);
				break;
			case ADC_CH7:
				data_pin.pin = 5;
				PINSEL_config(data_pin, ADC);
		}
	}
	else if(data_adc.adc == ADC_1)
	{
		switch(data_adc.channel)
		{
			case ADC_CH0:
				data_pin.pin = 6;
				PINSEL_config(data_pin, ADC);
				break;
			case ADC_CH1:
				data_pin.pin = 8;
				PINSEL_config(data_pin, ADC);
				break;
			case ADC_CH2:
				data_pin.pin = 10;
				PINSEL_config(data_pin, ADC);
				break;
			case ADC_CH3:
				data_pin.pin = 12;
				PINSEL_config(data_pin, ADC);
				break;
			case ADC_CH4:
				data_pin.pin = 13;
				PINSEL_config(data_pin, ADC);
				break;
			case ADC_CH5:
				data_pin.pin = 15;
				PINSEL_config(data_pin, ADC);
				break;
			case ADC_CH6:
				data_pin.pin = 21;
				PINSEL_config(data_pin, ADC);
				break;
			case ADC_CH7:
				data_pin.pin = 22;
				PINSEL_config(data_pin, ADC);
		}
	}
}

void ADC_enable(int adc)
{
	if(adc == ADC_0)
		AD0CR = 0x200000;
	else if(adc == ADC_1)
		AD1CR = 0x200000;
}

unsigned int ADC_read(struct analog data_adc)
{
	unsigned int value = 0;

	if(data_adc.adc == ADC_0)
	{
		AD0CR |= (1<<24)|(1<<data_adc.channel);
		while ((AD0DR&(1<<31))==0);
		AD0CR &= ~((1<<24)|(1<<data_adc.channel));
		value = (AD0DR>>6)&0x03FF;
	}
	else if(data_adc.adc == ADC_1)
	{
		AD1CR |= (1<<24)|(1<<data_adc.channel);
		while ((AD1DR&(1<<31))==0);
		AD1CR &= ~((1<<24)|(1<<data_adc.channel));
		value = (AD1DR>>6)&0x03FF;
	}

	return value;
}

char BUTTON_status(struct data button_data_pin)
{
	char var = GPIO_read(button_data_pin);

	if(var == '0')
		return '1';
	else // var = '1'
		return '0';
}

unsigned int GP2Y0A_read(struct analog data_adc)
{
	unsigned int adc_value, distance_value;

	adc_value = ADC_read(data_adc);

	if(adc_value > 23)
		distance_value = 25*((3000.0 / (adc_value - 23) + 0.3)); // Fórmula de conversión de tensión a mm.
	else
		distance_value = 800;

	if (distance_value > 800)
		distance_value = 800;
	else if (distance_value < 100)
		distance_value = 100;

	return distance_value;
}

unsigned int GP2D120_read(struct analog data_adc)
{
	unsigned int adc_value, distance_value;

	adc_value = ADC_read(data_adc);

	distance_value = 16.8*((2914.0 / (adc_value + 35) - 0.5)); // Fórmula de conversión de tensión a mm.

	if (distance_value > 350)
		distance_value = 350;
	else if (distance_value < 40)
		distance_value = 40;

	return distance_value;
}

char DISTANCE_digital_read(struct data sensor_data_pin)
{
	char var = GPIO_read(sensor_data_pin);

	if(var == '0')
		return '1';
	else // var = '1'
		return '0';
}

void EINT_config(struct data data_pin, char mode, char polar, unsigned int priority)
{
	int ext_int;
	unsigned int int_num;
	unsigned long name;

	if(data_pin.port == 0)
	{
		switch(data_pin.pin)
		{
			case 1:
				ext_int = 0;
				break;
			case 3:
				ext_int = 1;
				break;
			case 7:
				ext_int = 2;
				break;
			case 9:
				ext_int = 3;
				break;
			case 14:
				ext_int = 1;
				break;
			case 15:
				ext_int = 2;
				break;
			case 16:
				ext_int = 2;
				break;
			case 20:
				ext_int = 3;
				break;
			case 30:
				ext_int = 3;
		}
	}

	switch(ext_int)
	{
		case 0:
			int_num = 14;
			name = (unsigned long) EINT_0;
			break;
		case 1:
			int_num = 15;
			name = (unsigned long) EINT_1;
			break;
		case 2:
			int_num = 16;
			name = (unsigned long) EINT_2;
			break;
		default: // ext_int = 3
			int_num = 17;
			name = (unsigned long) EINT_3;
	}

	if(mode == LEVEL)
		EXTMODE &= ~(1<<ext_int);
	else // mode = EDGE
		EXTMODE |= 1<<ext_int;

	if(polar == DOWN)
		EXTPOLAR &= ~(1<<ext_int);
	else // polar = UP
		EXTPOLAR |= 1<<ext_int;

	PINSEL_config(data_pin, EINT);
	VIC_assign(name, priority, int_num);
}

float ENCODER_distance(float diameter, int edge_num, int cont)
{
	float distance;
	distance = pi * diameter * cont / edge_num;

	return distance;
}

float ENCODER_speed(float diameter, int edge_num, float time)
{
	float speed;

	speed = pi * diameter / (1000 * edge_num * time);

	return speed;
}

void GPIO_config(struct data data_pin, char inout)
{
	if(inout == IN)
	{
		if(data_pin.port == 0)
			IO0DIR &= ~(1<<data_pin.pin);
		else // port = 1
			IO1DIR &= ~(1<<data_pin.pin);
	}
	else // inout = OUT
	{
		if(data_pin.port == 0)
			IO0DIR |= 1<<data_pin.pin;
		else // port = 1
			IO1DIR |= 1<<data_pin.pin;
	}
}

void GPIO_set(struct data data_pin, unsigned char status)
{
	if(status == PIN_OFF)
	{
		if(data_pin.port == 0)
			IOCLR0 = 1<<data_pin.pin;
		else // port = 1
			IOCLR1 = 1<<data_pin.pin;
	}
	else // status = PIN_ON
	{
		if(data_pin.port == 0)
			IOSET0 = 1<<data_pin.pin;
		else // port = 1
			IOSET1 = 1<<data_pin.pin;
	}		
}

char GPIO_read(struct data data_pin)
{
	if(data_pin.port == 0)
	{
		if((IO0PIN&(1<<data_pin.pin)) == 0)
			return '0';
		else
			return '1';
	}
	else // port = 1
	{
		if((IO1PIN&(1<<data_pin.pin)) == 0)
			return '0';
		else
			return '1';
	}
}

void LED_set(struct data led_data_pin, unsigned char status)
{
	GPIO_set(led_data_pin, status);
}

unsigned int LINE_SENSOR_read(struct analog data_adc)
{
	unsigned int value;
	value = ADC_read(data_adc);
	return value;
}

char LIPO_TENSION_read(struct analog data_adc, float v_min)
{
	unsigned int value;
	value = ADC_read(data_adc);

	if(value < v_min*1024/3.3)
		return '1';
	else
		return '0';
}

void MOTOR_config(struct motor MOTOR)
{
	GPIO_config(MOTOR.bridge_h_pos_data_pin, OUT);
	GPIO_config(MOTOR.bridge_h_neg_data_pin, OUT);

	PWM_CH_config(MOTOR.channel);
}

void MOTOR_SPEED_set(struct motor MOTOR, float speed, char mode)
{
	if(mode == FORWARD)
	{
		GPIO_set(MOTOR.bridge_h_pos_data_pin, PIN_ON);
		GPIO_set(MOTOR.bridge_h_neg_data_pin, PIN_OFF);
	}
	else if(mode == BACKWARD)
	{
		GPIO_set(MOTOR.bridge_h_pos_data_pin, PIN_OFF);
		GPIO_set(MOTOR.bridge_h_neg_data_pin, PIN_ON);
	}
	else if(mode == LOCKED)
	{
		GPIO_set(MOTOR.bridge_h_pos_data_pin, PIN_ON);
		GPIO_set(MOTOR.bridge_h_neg_data_pin, PIN_ON);
	}
	else // mode == FREE
	{
		GPIO_set(MOTOR.bridge_h_pos_data_pin, PIN_OFF);
		GPIO_set(MOTOR.bridge_h_neg_data_pin, PIN_OFF);
	}

	PWM_set(MOTOR.channel, speed);
}

void PINSEL_config(struct data data_pin, char mode)
{
	if(data_pin.port == 0)
	{
		switch(data_pin.pin)
		{
			case 0:
				switch(mode)
				{
					case UART:
						PINSEL0 |= 1<<0;
						break;
					case PWM:
						PINSEL0 |= 2<<0;
				}
				break;
			case 1:
				switch(mode)
				{
					case UART:
						PINSEL0 |= 1<<2;
						break;
					case PWM:
						PINSEL0 |= 2<<2;
						break;
					case EINT:
						PINSEL0 |= 3<<2;
				}
				break;
			case 2:
				switch(mode)
				{
					case I2C:
						PINSEL0 |= 1<<4;
						break;
					case CAPTURE:
						PINSEL0 |= 2<<4;
				}
				break;
			case 3:
				switch(mode)
				{
					case I2C:
						PINSEL0 |= 1<<6;
						break;
					case MATCH:
						PINSEL0 |= 2<<6;
						break;
					case EINT:
						PINSEL0 |= 3<<6;
				}
				break;
			case 4:
				switch(mode)
				{
					case SPI:
						PINSEL0 |= 1<<8;
						break;
					case CAPTURE:
						PINSEL0 |= 2<<8;
						break;
					case ADC:
						PINSEL0 |= 3<<8;
				}
				break;
			case 5:
				switch(mode)
				{
					case SPI:
						PINSEL0 |= 1<<10;
						break;
					case MATCH:
						PINSEL0 |= 2<<10;
						break;
					case ADC:
						PINSEL0 |= 3<<10;
				}
				break;
			case 6:
				switch(mode)
				{
					case SPI:
						PINSEL0 |= 1<<12;
						break;
					case CAPTURE:
						PINSEL0 |= 2<<12;
						break;
					case ADC:
						PINSEL0 |= 3<<12;
				}
				break;
			case 7:
				switch(mode)
				{
					case SPI:
						PINSEL0 |= 1<<14;
						break;
					case PWM:
						PINSEL0 |= 2<<14;
						break;
					case EINT:
						PINSEL0 |= 3<<14;
				}
				break;
			case 8:
				switch(mode)
				{
					case UART:
						PINSEL0 |= 1<<16;
						break;
					case PWM:
						PINSEL0 |= 2<<16;
						break;
					case ADC:
						PINSEL0 |= 3<<16;
				}
				break;
			case 9:
				switch(mode)
				{
					case UART:
						PINSEL0 |= 1<<18;
						break;
					case PWM:
						PINSEL0 |= 2<<18;
						break;
					case EINT:
						PINSEL0 |= 3<<18;
				}
				break;
			case 10:
				switch(mode)
				{
					case UART:
						PINSEL0 |= 1<<20;
						break;
					case CAPTURE:
						PINSEL0 |= 2<<20;
						break;
					case ADC:
						PINSEL0 |= 3<<20;
				}
				break;
			case 11:
				switch(mode)
				{
					case UART:
						PINSEL0 |= 1<<22;
						break;
					case CAPTURE:
						PINSEL0 |= 2<<22;
						break;
					case I2C:
						PINSEL0 |= 3<<22;
				}
				break;
			case 12:
				switch(mode)
				{
					case UART:
						PINSEL0 |= 1<<24;
						break;
					case MATCH:
						PINSEL0 |= 2<<24;
						break;
					case ADC:
						PINSEL0 |= 3<<24;
				}
				break;
			case 13:
				switch(mode)
				{
					case UART:
						PINSEL0 |= 1<<26;
						break;
					case MATCH:
						PINSEL0 |= 2<<26;
						break;
					case ADC:
						PINSEL0 |= 3<<26;
				}
				break;
			case 14:
				switch(mode)
				{
					case UART:
						PINSEL0 |= 1<<28;
						break;
					case EINT:
						PINSEL0 |= 2<<28;
						break;
					case I2C:
						PINSEL0 |= 3<<28;
				}
				break;
			case 15:
				switch(mode)
				{
					case UART:
						PINSEL0 |= 1<<30;
						break;
					case EINT:
						PINSEL0 |= 2<<30;
						break;
					case ADC:
						PINSEL0 |= 3<<30;
				}
				break;
			case 16:
				switch(mode)
				{
					case EINT:
						PINSEL1 |= 1<<0;
						break;
					case MATCH:
						PINSEL1 |= 2<<0;
						break;
					case CAPTURE:
						PINSEL1 |= 3<<0;
				}
				break;
			case 17:
				switch(mode)
				{
					case CAPTURE:
						PINSEL1 |= 1<<2;
						break;
					case SPI:
						PINSEL1 |= 2<<2;
						break;
					case MATCH:
						PINSEL1 |= 3<<2;
				}
				break;
			case 18:
				switch(mode)
				{
					case CAPTURE:
						PINSEL1 |= 1<<4;
						break;
					case SPI:
						PINSEL1 |= 2<<4;
						break;
					case MATCH:
						PINSEL1 |= 3<<4;
				}
				break;
			case 19:
				switch(mode)
				{
					case MATCH:
						PINSEL1 |= 1<<6;
						break;
					case SPI:
						PINSEL1 |= 2<<6;
						break;
					case MATCH_2:
						PINSEL1 |= 3<<6;
				}
				break;
			case 20:
				switch(mode)
				{
					case MATCH:
						PINSEL1 |= 1<<8;
						break;
					case SPI:
						PINSEL1 |= 2<<8;
						break;
					case EINT:
						PINSEL1 |= 3<<8;
				}
				break;
			case 21:
				switch(mode)
				{
					case PWM:
						PINSEL1 |= 1<<10;
						break;
					case ADC:
						PINSEL1 |= 2<<10;
						break;
					case CAPTURE:
						PINSEL1 |= 3<<10;
				}
				break;
			case 22:
				switch(mode)
				{
					case ADC:
						PINSEL1 |= 1<<12;
						break;
					case CAPTURE:
						PINSEL1 |= 2<<12;
						break;
					case MATCH:
						PINSEL1 |= 3<<12;
				}
				break;
			case 25:
				switch(mode)
				{
					case ADC:
						PINSEL1 |= 1<<18;
						break;
					case DAC:
						PINSEL1 |= 2<<18;
				}
				break;
			case 26:
				switch(mode)
				{
					case ADC:
						PINSEL1 |= 1<<20;
				}
				break;
			case 27:
				switch(mode)
				{
					case ADC:
						PINSEL1 |= 1<<22;
						break;
					case CAPTURE:
						PINSEL1 |= 2<<22;
						break;
					case MATCH:
						PINSEL1 |= 3<<22;
				}
				break;
			case 28:
				switch(mode)
				{
					case ADC:
						PINSEL1 |= 1<<24;
						break;
					case CAPTURE:
						PINSEL1 |= 2<<24;
						break;
					case MATCH:
						PINSEL1 |= 3<<24;
				}
				break;
			case 29:
				switch(mode)
				{
					case ADC:
						PINSEL1 |= 1<<26;
						break;
					case CAPTURE:
						PINSEL1 |= 2<<26;
						break;
					case MATCH:
						PINSEL1 |= 3<<26;
				}
				break;
			case 30:
				switch(mode)
				{
					case ADC:
						PINSEL1 |= 1<<28;
						break;
					case EINT:
						PINSEL1 |= 2<<28;
						break;
					case CAPTURE:
						PINSEL1 |= 3<<28;
				}
				break;
		}
	}
}

void PWM_CH_config(int channel)
{
	struct data data_pin;

	if(channel == PWM1)
	{
		data_pin.port = 0;
		data_pin.pin = 0;
		PINSEL_config(data_pin, PWM);
		PWMLER |= 1<<1;
		PWMPCR |= 1<<9;
	}
	else if(channel == PWM2)
	{
		data_pin.port = 0;
		data_pin.pin = 7;
		PINSEL_config(data_pin, PWM);
		PWMLER |= 1<<2;
		PWMPCR |= 1<<10;
	}
	else if(channel == PWM3)
	{
		data_pin.port = 0;
		data_pin.pin = 1;
		PINSEL_config(data_pin, PWM);
		PWMLER |= 1<<3;
		PWMPCR |= 1<<11;
	}
	else if(channel == PWM4)
	{
		data_pin.port = 0;
		data_pin.pin = 8;
		PINSEL_config(data_pin, PWM);
		PWMLER |= 1<<4;
		PWMPCR |= 1<<12;
	}
	else if(channel == PWM5)
	{
		data_pin.port = 0;
		data_pin.pin = 21;
		PINSEL_config(data_pin, PWM);
		PWMLER |= 1<<5;
		PWMPCR |= 1<<13;
	}
	else if(channel == PWM6)
	{
		data_pin.port = 0;
		data_pin.pin = 9;
		PINSEL_config(data_pin, PWM);
		PWMLER |= 1<<6;
		PWMPCR |= 1<<14;
	}
}

void PWM_enable(float period)
{
	PWMTCR = 0x02;						// Reset TIMER y PREESCALER										   
	PWMPR = 0;							// El PRESCALER no modifica la frecuencia
	PWMMR0 = (float)PWM_PERIOD * Fpclk;	// Periodo PWM									   
	PWMLER |= 1<<0;						// Habilitar MATCH0
	PWMMCR = 0x02;						// Reset TIMER COUNTER REGISTER ON MATCH0							   
	PWMTCR = (1<<0)|(1<<3);				// Habilitar PWM y comenzar la cuenta
}

void PWM_set(int channel, float value)
{
	if(channel == 0)
		PWMMR0 = (float) value * Fpclk * PWM_PERIOD / 100;
	else if(channel == 1)
		PWMMR1 = (float) value * Fpclk * PWM_PERIOD / 100;
	else if(channel == 2)
		PWMMR2 = (float) value * Fpclk * PWM_PERIOD / 100;
	else if(channel == 3)
		PWMMR3 = (float) value * Fpclk * PWM_PERIOD / 100;
	else if(channel == 4)
		PWMMR4 = (float) value * Fpclk * PWM_PERIOD / 100;
	else if(channel == 5)
		PWMMR5 = (float) value * Fpclk * PWM_PERIOD / 100;
	else if(channel == 6)
		PWMMR6 = (float) value * Fpclk * PWM_PERIOD / 100;
}

void SERVO_config(int channel)
{
	PWM_CH_config(channel);
}

void SERVO_set (int channel, float position)
{
	if (position > 100)
		position = 100;
	else if(position < 0)
		position = 0;

	position = SERVO_MIN_POS + (position * (SERVO_MAX_POS - SERVO_MIN_POS)) / 100; 
	PWM_set(channel, position);
}

void MATCH_CHANNEL_config(char channel, float period)
{
	switch(channel)
	{
		case MATCH0_0:
			T0MCR |= 3<<0;
			T0MR0 = (unsigned long)(period*Fpclk);			
			break;
		case MATCH0_1:
			T0MCR |= 3<<3;
			T0MR1 = (unsigned long)(period*Fpclk);			
			break;
		case MATCH0_2:
			T0MCR |= 3<<6;
			T0MR2 = (unsigned long)(period*Fpclk);			
			break;
		case MATCH0_3:
			T0MCR |= 3<<9;
			T0MR3 = (unsigned long)(period*Fpclk);
			break;			
		case MATCH1_0:
			T1MCR |= 3<<0;
			T1MR0 = (unsigned long)(period*Fpclk);			
			break;
		case MATCH1_1:
			T1MCR |= 3<<3;
			T1MR1 = (unsigned long)(period*Fpclk);			
			break;
		case MATCH1_2:
			T1MCR |= 3<<6;
			T1MR2 = (unsigned long)(period*Fpclk);			
			break;
		case MATCH1_3:
			T1MCR |= 3<<9;
			T1MR3 = (unsigned long)(period*Fpclk);
	}
}

void CAPTURE_CHANNEL_config(char channel, struct data data_pin, char edge)
{
	PINSEL_config(data_pin, CAPTURE);

	switch(channel)
	{
		case CAPTURE0_0:
			T0CCR |= edge<<0;
			break;
		case CAPTURE0_1:
			T0CCR |= edge<<3;
			break;
		case CAPTURE0_2:
			T0CCR |= edge<<6;
			break;
		case CAPTURE0_3:
			T0CCR |= edge<<9;
			break;			
		case CAPTURE1_0:
			T1CCR |= edge<<0;
			break;
		case CAPTURE1_1:
			T1CCR |= edge<<3;
			break;
		case CAPTURE1_2:
			T1CCR |= edge<<6;
			break;
		case CAPTURE1_3:
			T1CCR |= edge<<9;
	}
}

void TIMER_config(char timer, char mode, unsigned int priority)
{
	int int_num;
	unsigned long name;

	if(mode == MATCH_MODE)
	{
		if(timer == TIMER0)
		{
			int_num = 4;
			name = (unsigned long) TIMER0_MATCH;

			T0TCR = 0x02;
			T0PR = 0;
			T0TCR = 0x01;
		}
		else // timer = TIMER1
		{
			int_num = 5;
			name = (unsigned long) TIMER1_MATCH;

			T1TCR = 0x02;
			T1PR = 0; 	
			T1TCR = 0x01;
		}
	}
	else // mode = CAPTURE_MODE
	{
		if(timer == TIMER0)
		{
			int_num = 4;
			name = (unsigned long) TIMER0_CAPTURE;

			T0TCR = 0x02;
			T0CTCR = 0;
			T0PR = 0;
			T0TCR = 0x01;
		}
		else // timer = TIMER1
		{
			int_num = 5;
			name = (unsigned long) TIMER1_CAPTURE;

			T1TCR = 0x02;
			T1CTCR = 0;
			T1PR = 0;
			T1TCR = 0x01;
		}
	}
	
	VIC_assign(name, priority, int_num);
}

void VIC_assign(unsigned long name, unsigned int priority, unsigned int num)
{
	switch(priority)
	{
		case 0:
			VICVectAddr0 = name;
			VICVectCntl0 = (0x20|num);
			break;
		case 1:
			VICVectAddr1 = name;
			VICVectCntl1 = (0x20|num);
			break;
		case 2:
			VICVectAddr2 = name;
			VICVectCntl2 = (0x20|num);
			break;
		case 3:
			VICVectAddr3 = name;
			VICVectCntl3 = (0x20|num);
			break;
		case 4:
			VICVectAddr4 = name;
			VICVectCntl4 = (0x20|num);
			break;
		case 5:
			VICVectAddr5 = name;
			VICVectCntl5 = (0x20|num);
			break;
		case 6:
			VICVectAddr6 = name;
			VICVectCntl6 = (0x20|num);
			break;
		case 7:
			VICVectAddr7 = name;
			VICVectCntl7 = (0x20|num);
			break;
		case 8:
			VICVectAddr8 = name;
			VICVectCntl8 = (0x20|num);
			break;
		case 9:
			VICVectAddr9 = name;
			VICVectCntl9 = (0x20|num);
			break;
		case 10:
			VICVectAddr10 = name;
			VICVectCntl10 = (0x20|num);
			break;
		case 11:
			VICVectAddr11 = name;
			VICVectCntl11 = (0x20|num);
			break;
		case 12:
			VICVectAddr12 = name;
			VICVectCntl12 = (0x20|num);
			break;
		case 13:
			VICVectAddr13 = name;
			VICVectCntl13 = (0x20|num);
			break;
		case 14:
			VICVectAddr14 = name;
			VICVectCntl14 = (0x20|num);
			break;
		default: // priority = 15
			VICVectAddr15 = name;
			VICVectCntl15 = (0x20|num);
	}

	VICIntEnable |= 1<<num;
}


#endif
