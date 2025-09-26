// DRV8874_C.h

#ifndef DRV8874_h
#define DRV8874_h

#include "Arduino.h"

#define PWM_FREQUENCY 20000
#define PWM_RESOLUTION 10 // 10 bits de resolucion, pero aumentable a 12 jj

typedef struct {
  uint8_t pinPM;
  uint8_t pinEN;
  uint8_t pinPH;
  uint8_t pinIP;
  uint8_t pinFault;

} DRV8874_Motor;

// --- Prototipos de las Funciones ---
// Inicializa los pines del motor
void motor_init(DRV8874_Motor *motor);

// Establece la velocidad y dirección del motor (-255 a 255)
void motor_setVelocidad(DRV8874_Motor *motor, int velocidad);
//Para el avance-- puede ser retroceso si le dan valores de velociadad negativo 
void motor_avance(DRV8874_Motor *motor, DRV8874_Motor *motor_L, int velocidad);
//Para el giro si flag es 1 debería girar a la izquierda, si flag es 0 a la derecha 
void giro_brusco(DRV8874_Motor *motor, DRV8874_Motor *motor_L, bool flag, int velocidad);
// Giro normal
void motor_giro(DRV8874_Motor *motor, DRV8874_Motor *motor_L, int velocidad, int velocidad_L );
// Devuelve la corriente medida en Amperios
float DRV8874_getCorriente(DRV8874_Motor *motor);

// Devuelve 'true' si el pin nFAULT indica un error
bool DRV8874_hayFallo(DRV8874_Motor *motor);

#endif