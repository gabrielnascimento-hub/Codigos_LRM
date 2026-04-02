#include <Adafruit_MotorShield.h>
#include <Wire.h>
#include <Arduino.h>

#define sensorEsquerda 8
#define sensorDireite 7
#define sensorCentral 6
#define trigPin  9          
#define echoPin  10

int AnalogicoEsquerda = A0; 
int valorDigitalEsquerda; 
int valorAnalogicoEsquerda;
 
int AnalogicoDireita = A1;
int valorDigitalDireita; 
int valorAnalogicoDireita;

int AnalogicoCentral = A2;
int valorDigitalCentral; 
int valorAnalogicoCentral;

float duration, distance;

bool BRANCO = 0, PRETO =1;

Adafruit_MotorShield AFMS = Adafruit_MotorShield();


  Adafruit_DCMotor *MotorEsquerdaFrente = AFMS.getMotor(1); //motor esq frente
  Adafruit_DCMotor *MotorEsquerdaTras = AFMS.getMotor(2); //motor esq tras
  Adafruit_DCMotor *MotorDireitaFrente = AFMS.getMotor(3); //motor direita frente
  Adafruit_DCMotor *MotorDireitaTras = AFMS.getMotor(4); //motor direita tras


void setup() {

  AFMS.begin();
  MotorEsquerdaFrente->setSpeed(60);
  MotorEsquerdaTras->setSpeed(60);
  MotorDireitaFrente->setSpeed(60);
  MotorDireitaTras->setSpeed(60);
  Serial.begin(9600);  
  pinMode(sensorEsquerda ,INPUT);
  pinMode(AnalogicoEsquerda ,INPUT);
  pinMode(sensorDireite ,INPUT);
  pinMode(AnalogicoDireita ,INPUT);
  pinMode(sensorCentral ,INPUT);
  pinMode(AnalogicoCentral ,INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}



void loop() {
  
valorDigitalEsquerda = digitalRead(sensorEsquerda); //Realiza a leitura digital e armazena o valor na variavel valorDigital
valorAnalogicoEsquerda = analogRead(AnalogicoEsquerda); //Realiza a leitura analógica e armazena o valor na variavel valorAnalogico
valorDigitalDireita = digitalRead(sensorDireite);
valorAnalogicoDireita = analogRead(AnalogicoDireita); 
valorDigitalCentral = digitalRead(sensorCentral);
valorAnalogicoCentral = analogRead(AnalogicoCentral); 

//Serial.println(valorDigital);
//Serial.println(valorDigitalDireita);
Serial.println(valorAnalogico);
Serial.println(valorAnalogicoDireita);
Serial.println("============");
delay(50);


  if ((valorDigitalCentral == PRETO) && (valorDigitalEsquerda == BRANCO) && (valorDigitalDireita == BRANCO)){
    frente(80);
    Parar(0);
    leituraSensorInfra();
  }
  else if ((valorDigitalDireita == PRETO) && (valorDigitalCentral == BRANCO)){
    correcaoEsquerda();
  }
  else if ((valorDigitalEsquerda == PRETO) && (valorDigitalCentral == BRANCO)){
    correcaoDireita();
  }
  else{
    Parar(3000);
  }

}

void leituraSensorInfra(){
  digitalRead(valorDigitalEsquerda);
  digitalRead(valorDigitalCentral);
  digitalRead(valorDigitalDireita)
}


void frente(int d){
  MotorEsquerdaFrente->run(BACKWARD);
  MotorEsquerdaTras->run(BACKWARD); 
  MotorDireitaFrente->run(BACKWARD);
  MotorDireitaTras->run(BACKWARD); 
  delay(d);
}

void Parar(int d){
    MotorEsquerdaFrente->run(RELEASE);
    MotorEsquerdaTras->run(RELEASE); 
    MotorDireitaFrente->run(RELEASE);
    MotorDireitaTras->run(RELEASE);    
    delay(d)
}

void tras(){
  MotorEsquerdaFrente->run(FORWARD);
  MotorEsquerdaTras->run(FORWARD); 
  MotorDireitaFrente->run(FORWARD);
  MotorDireitaTras->run(FORWARD);  
}

void correcaoEsquerda(){
  MotorEsquerdaFrente->run(BACKWARD);
  delay(140);
  Parar(0);
  leituraSensorInfra();
  delay(100);
}

void correcaoDireita(){
  MotorDireitaFrente->run(BACKWARD);
  delay(140);
  Parar(0);
  leituraSensorInfra();
  delay(100);
}
void leituraSensorUltrassonico(){
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration*.0343)/2;
    Serial.println(distance);
    delay(50);
}

