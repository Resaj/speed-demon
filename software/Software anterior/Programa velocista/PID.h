#ifndef PID_h
#define PID_h

//Variables PID
extern int sensors_average;
extern int sensors_sum;
extern float position;
extern float proportional;
extern float integral;
extern float derivative;
extern float last_proportional;
extern float control_value;
extern int max_difference;
extern float Kp;
extern float Ki;
extern float Kd;

#endif


// Funciones de control
void get_PID(void);
void set_motors(void);
void loop_PID(void);
