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
 
int AnalogicoDireita = A2;
int valorDigitalDireita; 
int valorAnalogicoDireita;

int AnalogicoCentral = A1;
int valorDigitalCentral; 
int valorAnalogicoCentral;

float duration, distance;

int leituraVerde = 0, leituraPreto = 0;
int cont = 0;
int verificao = 0;
int verde_min = 89, verde_max = 325;

bool BRANCO = 0, PRETO = 1;

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *MotorEsquerdaFrente = AFMS.getMotor(1); //motor esq frente
Adafruit_DCMotor *MotorEsquerdaTras = AFMS.getMotor(2);   //motor esq tras
Adafruit_DCMotor *MotorDireitaFrente = AFMS.getMotor(3);  //motor direita frente
Adafruit_DCMotor *MotorDireitaTras = AFMS.getMotor(4);   //motor direita tras

// Declaração das funções para o compilador mapear corretamente
void frente(int d);
void Parar(int d);
void tras();
void correcaoEsquerda();
void correcaoDireita();
void leituraSensorInfra();
void leituraSensorUltrassonico();
void tresBrancos(int d);
void CurvaEsquerda();
void CurvaDireita();
void Cruzamento();
void curva180();

void setup() {
  AFMS.begin();
  MotorEsquerdaFrente->setSpeed(100);
  MotorEsquerdaTras->setSpeed(100);
  MotorDireitaFrente->setSpeed(100);
  MotorDireitaTras->setSpeed(100);
  Serial.begin(9600);  
  pinMode(sensorEsquerda, INPUT);
  pinMode(AnalogicoEsquerda, INPUT);
  pinMode(sensorDireite, INPUT);
  pinMode(AnalogicoDireita, INPUT);
  pinMode(sensorCentral, INPUT);
  pinMode(AnalogicoCentral, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Atualiza as leituras no início do loop
  leituraSensorInfra();

  // Exibe os valores no Monitor Serial
  Serial.println(valorAnalogicoEsquerda);
  Serial.println(valorAnalogicoDireita);
  Serial.println("============");
  delay(50);

  if ((valorDigitalEsquerda == BRANCO) && (valorDigitalCentral == PRETO) && (valorDigitalDireita == BRANCO))
  {
    frente(100);
    Parar(0);
    leituraSensorInfra();
  }
  else if ((valorDigitalEsquerda == BRANCO) && (valorDigitalCentral == BRANCO) && (valorDigitalDireita == PRETO))
  {
    Parar(500);
    correcaoEsquerda();
    leituraSensorInfra();
  }
  else if ((valorDigitalEsquerda == PRETO) && (valorDigitalCentral == BRANCO) && (valorDigitalDireita == BRANCO))
  {
    Parar(500);
    correcaoDireita();
    leituraSensorInfra();
  }
  else if ((valorDigitalEsquerda == BRANCO) && (valorDigitalCentral == BRANCO) && (valorDigitalDireita == BRANCO))
  {
    Parar(20);
    tresBrancos(20);
    leituraSensorInfra();
  }
  else if ((valorDigitalEsquerda == PRETO) && (valorDigitalCentral == PRETO) && (valorDigitalDireita == BRANCO))
  {
    Parar(700);
    CurvaEsquerda();
  }
  else if ((valorDigitalEsquerda == BRANCO) && (valorDigitalCentral == PRETO) && (valorDigitalDireita == PRETO)) 
  {
    Parar(700);
    CurvaDireita();
  }
  else if((valorDigitalEsquerda == PRETO) && (valorDigitalCentral == PRETO) && (valorDigitalDireita == PRETO))
  {
    Parar(1000);
    frente(200);
    Parar(500);
    leituraSensorInfra();
    
    if((valorDigitalEsquerda == BRANCO) && (valorDigitalCentral == PRETO) && (valorDigitalDireita == BRANCO))
    {
      frente(450);
    }
    else if((valorDigitalEsquerda == BRANCO) && (valorDigitalCentral == BRANCO) && (valorDigitalDireita == BRANCO))
    {
      curva180();
    }
  } 
  else 
  {
    leituraSensorInfra();
  }
}

// Atualizado para salvar corretamente os valores nas variáveis globais
void leituraSensorInfra(){
  valorDigitalEsquerda = digitalRead(sensorEsquerda);
  valorDigitalCentral = digitalRead(sensorCentral);
  valorDigitalDireita = digitalRead(sensorDireite);
  valorAnalogicoEsquerda = analogRead(AnalogicoEsquerda);
  valorAnalogicoCentral = analogRead(AnalogicoCentral);
  valorAnalogicoDireita = analogRead(AnalogicoDireita);
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
  delay(d);
}

void tras(){
  MotorEsquerdaFrente->run(FORWARD);
  MotorEsquerdaTras->run(FORWARD); 
  MotorDireitaFrente->run(FORWARD);
  MotorDireitaTras->run(FORWARD);
}

void correcaoEsquerda(){
  MotorEsquerdaTras->run(BACKWARD);
  MotorEsquerdaFrente->run(BACKWARD);
  MotorDireitaFrente->run(RELEASE);
  MotorDireitaTras->run(RELEASE);
  delay(100);
  leituraSensorInfra();
}

void correcaoDireita(){
  MotorDireitaFrente->run(BACKWARD);
  MotorDireitaTras->run(BACKWARD);
  MotorEsquerdaFrente->run(RELEASE);
  MotorEsquerdaTras->run(RELEASE);
  delay(100);
  leituraSensorInfra();
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

void tresBrancos(int d){
  MotorDireitaFrente->setSpeed(60);
  MotorEsquerdaFrente->setSpeed(60);
  MotorEsquerdaFrente->run(BACKWARD);
  MotorEsquerdaTras->run(BACKWARD);
  MotorDireitaFrente->run(BACKWARD);
  MotorDireitaTras->run(BACKWARD);
  delay(d);
}

void CurvaEsquerda(){
  Parar(250);
  frente(1200);
  MotorDireitaFrente->run(BACKWARD);
  MotorDireitaTras->run(BACKWARD);
  MotorEsquerdaFrente->run(FORWARD);
  MotorEsquerdaTras->run(FORWARD);
  delay(3000);
}

void CurvaDireita(){
  Parar(250);
  frente(1200);
  MotorDireitaFrente->run(FORWARD);
  MotorDireitaTras->run(FORWARD);
  MotorEsquerdaFrente->run(BACKWARD);
  MotorEsquerdaTras->run(BACKWARD);
  delay(3000);
}

void Cruzamento(){
  Parar(250);
  frente(2000);
}

void curva180(){
  Parar(250);
  frente(700);
  MotorDireitaFrente->run(BACKWARD);
  MotorDireitaTras->run(BACKWARD);
  MotorEsquerdaFrente->run(FORWARD);
  MotorEsquerdaTras->run(FORWARD);
  delay(3000);
}
