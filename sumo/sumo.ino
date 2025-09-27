#include <DRV8874.h>
#include <WiFi.h>
#include <WebServer.h>


#define PM 38    // PMODE: HIGH para modo PWM
#define EN 21     // IN21 en modo PWM derecho
#define PH 20     // IN20 en modo PWM derecho
#define IP 8     // IPROPI: corriente analógica
#define Fault 42 // nFAULT: LOW indica error

#define PM_L 47    
#define EN_L 1 //IN1 en modo PWM izquierdo  
#define PH_L 2 //IN2 en modo PWM izquierdo
#define IP_L 10    
#define Fault_L 19 

//Entradas digitales de los sensores detectores de oponentes 
#define SDer 6 
#define SCder 7
#define SCizq 4
#define SIzq 5

//Sensores de linea
#define MLIzq 9

//Sensor IR
#define SIR 13 

const char* ssid = "Moñai";
const char* password = "travaenjoyer";
WebServer server(80);

enum class STATES {IDLE, TEST, SEARCH, ATTACK_D,ATTACK_H, ATTACK_L} SUMO;
bool driverState = false;

DRV8874_Motor MI = {PM_L,EN_L,PH_L,IP_L , Fault_L};
DRV8874_Motor MD = {PM, EN, PH, IP, Fault};

// --- Para PID ---
int sensores[4];
float error= 0.0;
const int velocidadBase = 100;

;
const int kp = 2;
int measureTime = 15; // cada 15 ms va a tomar una lectura 
const static int disparo = 1;

void handleRoot();
void handleOn();
void handleOff();
void handleNotFound();

int sumaLecturas(){
  sensores[0] = digitalRead(SIzq);
  sensores[1] = digitalRead(SCizq);
  sensores[2] = digitalRead(SCder);
  sensores[3] = digitalRead(SDer);
    return sensores[0] + sensores[1] + sensores[2] + sensores[3]; 
}

void errorDireccion() {
  int fizq = sensores[0] + sensores[1] ;
  int fder = sensores[2] + sensores[3] ;
  int total = fizq + fder;
  if(total == 0){
    error = 0;
  } else {
    error = (fder - fizq)/(float)total;
  }
}

// ----- Funcion para calcular los ajuste a los motores --- 
void ajustarMotor(DRV8874_Motor *MD, DRV8874_Motor *MI){
  float correccion = error * kp * velocidadBase;

  int velD = constrain(velocidadBase - correccion, -150, 150);
  int velI = constrain(velocidadBase + correccion, -150, 150); 
  motor_giro(MD,MI, velD, velI);
}


void setup() {
  Serial.begin(115200);
  //Sensores de oponente
  pinMode(SDer, INPUT);
  pinMode(SCder, INPUT);
  pinMode(SCizq, INPUT);
  pinMode(SIzq, INPUT);

  //Sensores de línea 
  pinMode(MLIzq, INPUT);
  //Sensor IR
  pinMode(SIR, INPUT);

  motor_init(&MD);
  motor_init(&MI);
  
  if(WiFi.softAP(ssid, password)){
    Serial.println("Access Point Ip ");
    Serial.print(WiFi.softAPIP());
  } else {
    Serial.println("Fatal error in the SoftAP");
  }

  server.on("/", HTTP_GET, handleRoot);     // Main page
  server.on("/on", HTTP_GET, handleOn);     // Action to turn on
  server.on("/off", HTTP_GET, handleOff);   // Action to turn off
  server.onNotFound(handleNotFound);        // 404 handler
  server.begin();

  SUMO = STATES::IDLE;
}

void loop() {
  server.handleClient();
  switch(SUMO) {
    case STATES::IDLE: {
      if(digitalRead(SIR)){
        Serial.print("IR Encendido ==>  ");
        Serial.println(digitalRead(SIR));
        SUMO=STATES:: TEST;
        break;
      } else{
        // Asegurar que los motores están APAGADOS en IDLE
        motor_avance(&MD, &MI,0);
      }
     break;

    }
    // ------ test ------
    case STATES:: TEST: {
      if(sumaLecturas() >= disparo){
        errorDireccion();
        ajustarMotor(&MD, &MI);

      } else{
        motor_avance(&MD, &MI, 0);

      }
      if(digitalRead(SIR) == 0){
        motor_avance(&MD, &MI, 0);
        SUMO = STATES :: IDLE;
        break;
      }
    break;
      
    }
    // ------ Declaramos un estado en el cual se va a encargar de la busqueda ---
    case STATES :: SEARCH:{
      
    }
    // ------ Ataque que Danis va a programar ---------
    case STATES:: ATTACK_D: {
      if(digitalRead(SIR)==0){
        Serial.print("IR Apagado ==>  ");
        Serial.println(digitalRead(SIR));
        motor_avance(&MD, &MI, 0);
        SUMO=STATES::IDLE;
        break;
      }
      if(digitalRead(SCder)==1 && digitalRead(SCizq)==1){
      analogWrite(EN, 120);     
      analogWrite(PH, 0);

      analogWrite(EN_L, 120);     
      analogWrite(PH_L, 0);
      Serial.println("Adelante");
      

      } 
      else if(digitalRead(SDer)==1){ //Si detectamos un 1 en el sensor de la derecha, giramos el minisumo hasta estar frente a frente nuevamente contra el oponente
        analogWrite(EN, 0);     
        analogWrite(PH, 0);
        
        analogWrite(EN_L, 120);     
        analogWrite(PH_L, 0);
        Serial.println("Giro a la izquierda");
        
      } else if(digitalRead(SIzq)==1){//Si detectamos un 1 en el sensor de la izquierda, giramos el minisumo hasta estar frente a frente nuevamente contra el oponente
        analogWrite(EN, 120);     
        analogWrite(PH, 0);

        analogWrite(EN_L, 0);     
        analogWrite(PH_L, 0);

        Serial.println("Giro a la derecha");
        

      } else{ //Mientras no detecte nada en su frente o costados, el minisumo no hace nada 
        motor_avance(&MD,&MI,0);

      }
        break;
    }
    //------ Ataque que Lucas va a programar --------
    case STATES::ATTACK_L:{
      break;
    }

    // ----- Ataque Hernán ------
    case STATES:: ATTACK_H :{
      
      break;
    }
  }
  
}