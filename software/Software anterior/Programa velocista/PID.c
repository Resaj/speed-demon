#include <lpc213x.h>
#include "Declaraciones.h"
#include "PID.h"

/********************** ALGORITMO PID **********************/
void get_PID(void)
{
	sensors_average = 0;
	for(i = 0; i < S; i++)
		sensors_average += LINEAresult[i]*i*1000;

	sensors_sum = 0;
	for(i = 0; i < S; i++)
		sensors_sum += LINEAresult[i];

	if(sensors_sum == 0)
	{
		if(last_proportional > 0)
			position = S*1000;
		else // last_proportional < 0
			position = 0;
	}
	else // sensors_sum != 0
		position = (sensors_average / sensors_sum);

	proportional = position - (S-1)*1000/2;
	integral = integral + proportional;
	derivative = proportional - last_proportional;
	last_proportional = proportional;

	control_value = (float)(proportional * Kp + integral * Ki + derivative * Kd);
}

void set_motors(void)
{
	if(sensors_sum == 0)
	{
		if(control_value > 0)
		{
			cont = 0;
			velocidad = v_lenta;

			IO1PIN &= ~((1<<22)|(1<<25));
			IO1PIN |= (1<<28)|(1<<30);
			PWMMR2 = (float)velocidad*Per_pwm*Fpclk;
		}
		else if(control_value < 0)
		{
			cont = 0;
			velocidad = v_lenta;

			IO1PIN &= ~((1<<28)|(1<<30));
			IO1PIN |= (1<<22)|(1<<25);
			PWMMR6 = (float)velocidad*Per_pwm*Fpclk;
		}
	}
	else
	{
		if(control_value > max_difference)
			control_value = max_difference;
		else if(control_value < -max_difference)
			control_value = -max_difference;


		if(control_value > max_difference/2)
		{
			if(frenar == 1)
			{
				IO1PIN |= (1<<22);
				IO1PIN |= (1<<28);
				PWMMR6 = (float)velocidad*Per_pwm*Fpclk;
				PWMMR2 = (float)(control_value - max_difference/2)/(max_difference/2) * velocidad*Per_pwm*Fpclk;
			}
			else
			{
				IO1PIN &= ~(1<<22);
				IO1PIN |= (1<<28);
				PWMMR2 = (float)velocidad*Per_pwm*Fpclk;
				PWMMR6 = (float)(control_value - max_difference/2)/(max_difference/2) * velocidad*Per_pwm*Fpclk;
			}
		}
		else if(control_value < -max_difference/2)
		{
			if(frenar == 1)
			{
				IO1PIN |= (1<<22);
				IO1PIN |= (1<<28);

				PWMMR6 = (float)(-control_value - max_difference/2)/(max_difference/2) * velocidad*Per_pwm*Fpclk;
				PWMMR2 = (float)velocidad*Per_pwm*Fpclk;
			}
			else
			{
				IO1PIN &= ~(1<<28);
				IO1PIN |= (1<<22);

				PWMMR2 = (float)(-control_value - max_difference/2)/(max_difference/2) * velocidad*Per_pwm*Fpclk;
				PWMMR6 = (float)velocidad*Per_pwm*Fpclk;
			}
		}
		else
		{
			IO1PIN &= ~((1<<28)|(1<<22));
			IO1PIN |= (1<<25)|(1<<30);
	
			if(control_value < 0)
			{
				if(frenar == 1)
				{
					IO1PIN |= (1<<22);
					IO1PIN |= (1<<28);

					PWMMR2 = (float)velocidad*Per_pwm*Fpclk;
					PWMMR6 = (float)(max_difference + control_value)/max_difference * velocidad*Per_pwm*Fpclk;
				}
				else
				{
					PWMMR2 = (float)(max_difference + control_value)/max_difference * velocidad*Per_pwm*Fpclk;
					PWMMR6 = (float)velocidad*Per_pwm*Fpclk;
				}
			}
			else if(control_value > 0)
			{
				if(frenar == 1)
				{
					IO1PIN |= (1<<22);
					IO1PIN |= (1<<28);

					PWMMR2 = (float)(max_difference - control_value)/max_difference * velocidad*Per_pwm*Fpclk;
					PWMMR6 = (float)velocidad*Per_pwm*Fpclk;

				}
				else
				{	
					PWMMR2 = (float)velocidad*Per_pwm*Fpclk;
					PWMMR6 = (float)(max_difference - control_value)/max_difference * velocidad*Per_pwm*Fpclk;
				}
			}
			else
			{
				if(frenar == 1)
				{
					IO1PIN |= (1<<22);
					IO1PIN |= (1<<28);

					PWMMR2 = velocidad*Per_pwm*Fpclk;
					PWMMR6 = velocidad*Per_pwm*Fpclk;					
				}
				else
				{	
					PWMMR2 = velocidad*Per_pwm*Fpclk;
					PWMMR6 = velocidad*Per_pwm*Fpclk;
				}
			}
		}
	}

	PWMLER = (1<<0)|(1<<2)|(1<<6);
}

void loop_PID(void)
{
	get_PID();
	set_motors();
}
