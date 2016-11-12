#ifndef MOTORS_H
#define MOTORS_H

// Variables para el control de velocidad
extern float velocidad;
extern float v_izq_real, v_der_real, vl_real, w_real;
extern char mode_izq, mode_der;


//Variables del PID de velocidad angular
extern float proportional;
extern float integral;
extern float integral_max;
extern float w_ctrl_max;
extern float Kp;
extern float Ki;
extern float Kd;

// Funciones de control
float get_W(void);
float get_VL(void);
void set_motors(float vl, float w);


#endif
