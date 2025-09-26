//A LLUCAS LE FUNCIONA

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
#define MLDer 48

//Sensor IR
#define SIR 13 

// current measure 
float vip = 0;
float current = 0;

float vip_L = 0;
float current_L = 0;


const char* ssid = "Moñai";
const char* password = "travaenjoyer";
WebServer server(80);

enum class STATES {IDLE, TEST, ATTACK, ATTACK_2} SUMO;
bool driverState = false;

void handleRoot();
void handleOn();
void handleOff();
void handleNotFound();


void setup() {
  Serial.begin(115200);

  pinMode(PM, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(PH, OUTPUT);
  pinMode(IP, INPUT);
  pinMode(Fault, INPUT);

  pinMode(PM_L, OUTPUT);
  pinMode(EN_L, OUTPUT);
  pinMode(PH_L, OUTPUT);
  pinMode(IP_L, INPUT);
  pinMode(Fault_L, INPUT);

  //Sensores de oponente
  pinMode(SDer, INPUT);
  pinMode(SCder, INPUT);
  pinMode(SCizq, INPUT);
  pinMode(SIzq, INPUT);

  //Sensores de línea 
  pinMode(MLIzq, INPUT);
  pinMode(MLDer, INPUT);

  //Sensor IR
  pinMode(SIR, INPUT);

  //Modo PWM
  digitalWrite(PM, HIGH);
  digitalWrite(PM_L, HIGH);

  analogWrite(EN, 0);     
  analogWrite(PH, 0);
  analogWrite(EN_L, 0);     
  analogWrite(PH_L, 0);
  
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

void apagarMotores(){
  analogWrite(EN, 0);     
  analogWrite(PH, 0);
  analogWrite(EN_L, 0);     
  analogWrite(PH_L, 0);
}

void loop() {
  server.handleClient();
  switch(SUMO) {
    case STATES::IDLE: {
      if(digitalRead(SIR)==1){
        Serial.print("IR Encendido ==>  ");
        Serial.println(digitalRead(SIR));
        SUMO=STATES::ATTACK;
        break;
      } else{
        // Asegurar que los motores están APAGADOS en IDLE
        apagarMotores();
      }
     break;

    }
    case STATES:: ATTACK: {
    if(digitalRead(SIR)==0){
      Serial.print("IR Apagado ==>  ");
      Serial.println(digitalRead(SIR));
      apagarMotores();
      SUMO=STATES::IDLE;
      break;
    }
    if(digitalRead(SCder)==1 && digitalRead(SCizq)==1){
    analogWrite(EN, 120);     
    analogWrite(PH, 0);

    analogWrite(EN_L, 120);     
    analogWrite(PH_L, 0);
    Serial.println("Adelante");
    //Serial.printf("Sd+: %u SCd: %u SCi: %u Si+: %u ", digitalRead(SDer), digitalRead(SCder), digitalRead(SCizq), digitalRead(SIzq) );

  } 
  else if(digitalRead(SDer)==1){ //Si detectamos un 1 en el sensor de la derecha, giramos el minisumo hasta estar frente a frente nuevamente contra el oponente
    analogWrite(EN, 0);     
    analogWrite(PH, 0);
    
    analogWrite(EN_L, 120);     
    analogWrite(PH_L, 0);
    Serial.println("Giro a la izquierda");
    //Serial.printf("Sd+: %u SCd: %u SCi: %u Si+: %u ", digitalRead(SDer), digitalRead(SCder), digitalRead(SCizq), digitalRead(SIzq) );
  } else if(digitalRead(SIzq)==1){//Si detectamos un 1 en el sensor de la izquierda, giramos el minisumo hasta estar frente a frente nuevamente contra el oponente
    analogWrite(EN, 120);     
    analogWrite(PH, 0);

    analogWrite(EN_L, 0);     
    analogWrite(PH_L, 0);

    Serial.println("Giro a la derecha");
    //Serial.printf("Sd+: %u SCd: %u SCi: %u Si+: %u ", digitalRead(SDer), digitalRead(SCder), digitalRead(SCizq), digitalRead(SIzq) );

  } else{ //Mientras no detecte nada en su frente o costados, el minisumo no hace nada 
    apagarMotores();

  }
      break;
    }
  }
  
  
  /*server.handleClient();
  if(driverState){
  analogWrite(EN, 150);     
  analogWrite(PH, 0);
  analogWrite(EN_L, 150);     
  analogWrite(PH_L, 0);
  } else {
  analogWrite(EN, 0);     
  analogWrite(PH, 0);
  analogWrite(EN_L, 0);     
  analogWrite(PH_L, 0);
  }*/

}
