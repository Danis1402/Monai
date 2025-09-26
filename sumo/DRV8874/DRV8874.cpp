
#include "DRV8874.h"

// Implementación de la función de inicialización
void motor_init(DRV8874_Motor *motor) {
  pinMode(motor->pinPM, OUTPUT);
  pinMode(motor->pinIP, INPUT);
  pinMode(motor->pinFault, INPUT_PULLUP); // Recomendado para nFAULT

  digitalWrite(motor->pinPM, HIGH); // Para que siempre sea el modo PWM jj
 // inicializar los canales 
 // configuramos al pin que queremos 
  ledcAttach(motor->pinEN, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttach(motor->pinPH, PWM_FREQUENCY, PWM_RESOLUTION);
 
 motor_setVelocidad(motor,0);

}

// Implementación del control de velocidad
void motor_setVelocidad(DRV8874_Motor *motor, int velocidad) {
  // Limitar la velocidad al rango de PWM (-255 a 255)
  velocidad = constrain(velocidad, -255, 255);

  const int maxDutyCycle = (1 << PWM_RESOLUTION)-1;
  if (velocidad > 0) {
    // Hacia adelante
    int dutyCycle = map(velocidad,0,255, 0, maxDutyCycle);
    ledcWrite(motor->pinEN,dutyCycle);
    ledcWrite(motor->pinPH,0);
  } else if (velocidad < 0) {
    // Hacia atrás
    int dutyCycle = map(-velocidad,0,255, 0, maxDutyCycle);
    ledcWrite(motor->pinEN,0);
    ledcWrite(motor->pinPH,dutyCycle); // Usamos el valor absoluto
  } else {
    // Parar (frenado rápido)
    ledcWrite(motor->pinEN, 0);
    ledcWrite(motor->pinPH, 0);
  }
}
// aca el resto de funciones "hereda" de la primera función y facilita la vida
void motor_avance(DRV8874_Motor *motor, DRV8874_Motor *motor_L, int velocidad){
  motor_setVelocidad(motor, velocidad);
  motor_setVelocidad(motor_L, velocidad);
}

void giro_brusco(DRV8874_Motor *motor, DRV8874_Motor *motor_L, bool flag, int velocidad){ // flag = 1 izq, k --> proporción en la cual va a rotar qué tan brusco será el gira
  
  velocidad = abs(velocidad);
  if(flag){
   motor_setVelocidad(motor, velocidad);
   motor_setVelocidad(motor_L, -velocidad);
  }else{
   motor_setVelocidad(motor, -velocidad);
   motor_setVelocidad(motor_L, velocidad);
  }
}
//Reversa
void motor_giro(DRV8874_Motor *motor, DRV8874_Motor *motor_L, int velocidad, int velocidad_L ){
  // --- vacio por el momento si usamos delay o no 
  motor_setVelocidad(motor, velocidad);
  motor_setVelocidad(motor_L, velocidad_L);
}

// Implementación de la lectura de corriente
float DRV8874_getCorriente(DRV8874_Motor *motor) {
  // NOTA: Ajusta Vref si tu ESP32 usa 3.3V como referencia para el ADC
  float Vref = 2.5; 
  float Vip = (Vref * analogRead(motor->pinIP)) / 4095.0;
  return Vip / 0.45; // Tu factor de calibración
}

// Implementación de la lectura de fallo
bool DRV8874_hayFallo(DRV8874_Motor *motor) {
  // nFAULT es activo bajo, por lo que un LOW (0) indica un fallo.
  return digitalRead(motor->pinFault) == LOW;
}