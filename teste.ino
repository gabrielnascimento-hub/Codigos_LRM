#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>

// =====================================================
// PINOS DOS SENSORES
// =====================================================

#define sensorEsquerda 8
#define sensorDireita  7
#define sensorCentral  6

#define AnalogicoEsquerda A0
#define AnalogicoCentral  A1
#define AnalogicoDireita  A2

// =====================================================
// SENSOR ULTRASSÔNICO
// =====================================================

#define trigPin 10
#define echoPin 11

// Distância mínima para parar o robô
// em centímetros
const float distanciaObstaculo = 15.0;

// Intervalo entre leituras do ultrassônico
// em milissegundos
const unsigned long intervaloUltrassonico = 50;

unsigned long ultimaLeituraUltrassonico = 0;

// =====================================================
// ESTADOS DOS SENSORES
// =====================================================

// ATENÇÃO:
// Se o teste mostrar que o sensor fornece 0 no PRETO
// e 1 no BRANCO, troque para:
//
// const int BRANCO = 1;
// const int PRETO  = 0;

const int BRANCO = 0;
const int PRETO  = 1;

// =====================================================
// LEITURAS DOS SENSORES
// =====================================================

int valorDigitalEsquerda = 0;
int valorDigitalCentral  = 0;
int valorDigitalDireita  = 0;

int valorAnalogicoEsquerda = 0;
int valorAnalogicoCentral  = 0;
int valorAnalogicoDireita  = 0;

// =====================================================
// ULTRASSÔNICO
// =====================================================

unsigned long duration = 0;
float distance = 999.0;

// Indica se existe obstáculo
bool obstaculoDetectado = false;

// =====================================================
// CONTADORES
// =====================================================

int leituraVerde = 0;
int leituraPreto = 0;
int cont = 0;

// =====================================================
// CALIBRAÇÃO DO VERDE
// =====================================================

// Esses valores são apenas iniciais.
// Devem ser ajustados de acordo com seus sensores.

const int verde_min = 89;
const int verde_max = 325;

// =====================================================
// MOTOR SHIELD
// =====================================================

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *MotorEsquerdaFrente = AFMS.getMotor(1);
Adafruit_DCMotor *MotorEsquerdaTras   = AFMS.getMotor(2);

Adafruit_DCMotor *MotorDireitaFrente  = AFMS.getMotor(3);
Adafruit_DCMotor *MotorDireitaTras    = AFMS.getMotor(4);

// =====================================================
// VELOCIDADE DOS MOTORES
// =====================================================

const int velocidadeNormal = 60;
const int velocidadeLenta  = 45;

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(9600);

  // -----------------------------
  // Sensores digitais
  // -----------------------------

  pinMode(sensorEsquerda, INPUT);
  pinMode(sensorCentral, INPUT);
  pinMode(sensorDireita, INPUT);

  // -----------------------------
  // Sensores analógicos
  // -----------------------------

  pinMode(AnalogicoEsquerda, INPUT);
  pinMode(AnalogicoCentral, INPUT);
  pinMode(AnalogicoDireita, INPUT);

  // -----------------------------
  // Ultrassônico
  // -----------------------------

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  digitalWrite(trigPin, LOW);

  // -----------------------------
  // Motor Shield
  // -----------------------------

  if (!AFMS.begin()) {

    Serial.println("ERRO: Motor Shield nao encontrado!");

    while (1);
  }

  // -----------------------------
  // Velocidades
  // -----------------------------

  MotorEsquerdaFrente->setSpeed(velocidadeNormal);
  MotorEsquerdaTras->setSpeed(velocidadeNormal);

  MotorDireitaFrente->setSpeed(velocidadeNormal);
  MotorDireitaTras->setSpeed(velocidadeNormal);

  Parar(0);

  Serial.println("==============================");
  Serial.println("ROBO INICIADO");
  Serial.println("ULTRASSONICO ATIVO");
  Serial.println("==============================");

  delay(1000);
}

// =====================================================
// LOOP PRINCIPAL
// =====================================================

void loop() {

  // ===================================================
  // ATUALIZA SENSOR DE LINHA
  // ===================================================

  leituraSensorInfra();

  // ===================================================
  // ATUALIZA ULTRASSÔNICO
  // ===================================================

  atualizarUltrassonico();

  // ===================================================
  // SE HOUVER OBSTÁCULO, PARA
  // ===================================================

  if (obstaculoDetectado) {

    Parar(0);

    return;
  }

  // ===================================================
  // DEBUG DOS SENSORES
  // ===================================================

  Serial.print("Digital: ");

  Serial.print("E=");
  Serial.print(valorDigitalEsquerda);

  Serial.print(" C=");
  Serial.print(valorDigitalCentral);

  Serial.print(" D=");
  Serial.print(valorDigitalDireita);

  Serial.print(" | Analogico: ");

  Serial.print("E=");
  Serial.print(valorAnalogicoEsquerda);

  Serial.print(" C=");
  Serial.print(valorAnalogicoCentral);

  Serial.print(" D=");
  Serial.print(valorAnalogicoDireita);

  Serial.print(" | Distancia=");

  Serial.print(distance);

  Serial.println(" cm");

  // ===================================================
  // E = BRANCO
  // C = PRETO
  // D = BRANCO
  //
  // Linha está no centro
  // ===================================================

  if (
    valorDigitalEsquerda == BRANCO &&
    valorDigitalCentral == PRETO &&
    valorDigitalDireita == BRANCO
  ) {

    frente(30);
  }

  // ===================================================
  // E = BRANCO
  // C = BRANCO
  // D = PRETO
  //
  // Linha está à direita
  // Corrigir para esquerda
  // ===================================================

  else if (
    valorDigitalEsquerda == BRANCO &&
    valorDigitalCentral == BRANCO &&
    valorDigitalDireita == PRETO
  ) {

    correcaoEsquerda();
  }

  // ===================================================
  // E = PRETO
  // C = BRANCO
  // D = BRANCO
  //
  // Linha está à esquerda
  // Corrigir para direita
  // ===================================================

  else if (
    valorDigitalEsquerda == PRETO &&
    valorDigitalCentral == BRANCO &&
    valorDigitalDireita == BRANCO
  ) {

    correcaoDireita();
  }

  // ===================================================
  // E = BRANCO
  // C = BRANCO
  // D = BRANCO
  //
  // Nenhum sensor encontrou a linha
  // ===================================================

  else if (
    valorDigitalEsquerda == BRANCO &&
    valorDigitalCentral == BRANCO &&
    valorDigitalDireita == BRANCO
  ) {

    tresBrancos(20);
  }

  // ===================================================
  // E = PRETO
  // C = PRETO
  // D = BRANCO
  //
  // Possível curva para esquerda / verde
  // ===================================================

  else if (
    valorDigitalEsquerda == PRETO &&
    valorDigitalCentral == PRETO &&
    valorDigitalDireita == BRANCO
  ) {

    Parar(50);

    tras();
    delay(50);

    Parar(50);

    leituraSensorInfra();

    if (detectouVerdeEsquerda()) {

      Serial.println("VERDE ESQUERDA");

      CurvaEsquerda();
    }
  }

  // ===================================================
  // E = BRANCO
  // C = PRETO
  // D = PRETO
  //
  // Possível curva para direita / verde
  // ===================================================

  else if (
    valorDigitalEsquerda == BRANCO &&
    valorDigitalCentral == PRETO &&
    valorDigitalDireita == PRETO
  ) {

    Parar(50);

    tras();
    delay(50);

    Parar(50);

    leituraSensorInfra();

    if (detectouVerdeDireita()) {

      Serial.println("VERDE DIREITA");

      CurvaDireita();
    }
  }

  // ===================================================
  // E = PRETO
  // C = PRETO
  // D = PRETO
  //
  // Cruzamento ou curva de 180°
  // ===================================================

  else if (
    valorDigitalEsquerda == PRETO &&
    valorDigitalCentral == PRETO &&
    valorDigitalDireita == PRETO
  ) {

    verificarCruzamento();
  }

  // ===================================================
  // SEGURANÇA
  // ===================================================

  else {

    leituraSensorInfra();
  }

  delay(5);
}

// =====================================================
// LEITURA DOS SENSORES
// =====================================================

void leituraSensorInfra() {

  // -----------------------------
  // Sensores digitais
  // -----------------------------

  valorDigitalEsquerda = digitalRead(sensorEsquerda);

  valorDigitalCentral = digitalRead(sensorCentral);

  valorDigitalDireita = digitalRead(sensorDireita);

  // -----------------------------
  // Sensores analógicos
  // -----------------------------

  valorAnalogicoEsquerda = analogRead(AnalogicoEsquerda);

  valorAnalogicoCentral = analogRead(AnalogicoCentral);

  valorAnalogicoDireita = analogRead(AnalogicoDireita);
}

// =====================================================
// ULTRASSÔNICO
// =====================================================

void atualizarUltrassonico() {

  // Evita ficar lendo o ultrassônico em todos
  // os ciclos do controle de linha.

  if (
    millis() - ultimaLeituraUltrassonico <
    intervaloUltrassonico
  ) {

    return;
  }

  ultimaLeituraUltrassonico = millis();

  // -----------------------------
  // Dispara o sensor
  // -----------------------------

  digitalWrite(trigPin, LOW);

  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);

  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  // -----------------------------
  // Mede o eco
  // -----------------------------
  //
  // Timeout de 10 ms.
  // Aproximadamente até 1,7 m.
  //

  duration = pulseIn(
    echoPin,
    HIGH,
    10000
  );

  // -----------------------------
  // Se não recebeu eco
  // -----------------------------

  if (duration == 0) {

    distance = 999.0;

    obstaculoDetectado = false;

    return;
  }

  // -----------------------------
  // Calcula distância
  // -----------------------------

  distance = (duration * 0.0343) / 2.0;

  // -----------------------------
  // Verifica obstáculo
  // -----------------------------

  if (distance <= distanciaObstaculo) {

    obstaculoDetectado = true;

    Serial.print("!!! OBSTACULO !!! Distancia: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  else {

    obstaculoDetectado = false;
  }
}

// =====================================================
// VERIFICA VERDE NA ESQUERDA
// =====================================================

bool detectouVerdeEsquerda() {

  return (
    valorAnalogicoEsquerda >= verde_min &&
    valorAnalogicoEsquerda <= verde_max
  );
}

// =====================================================
// VERIFICA VERDE NA DIREITA
// =====================================================

bool detectouVerdeDireita() {

  return (
    valorAnalogicoDireita >= verde_min &&
    valorAnalogicoDireita <= verde_max
  );
}

// =====================================================
// VERIFICA CRUZAMENTO
// =====================================================

void verificarCruzamento() {

  Serial.println("TODOS OS SENSORES PRETOS");

  Parar(100);

  leituraPreto = 0;
  leituraVerde = 0;
  cont = 0;

  // Faz 10 leituras para confirmar

  while (cont < 10) {

    leituraSensorInfra();

    // -----------------------------------------
    // Conta leituras totalmente pretas
    // -----------------------------------------

    if (
      valorDigitalEsquerda == PRETO &&
      valorDigitalCentral == PRETO &&
      valorDigitalDireita == PRETO
    ) {

      leituraPreto++;
    }

    // -----------------------------------------
    // Verifica verde
    // -----------------------------------------

    if (
      detectouVerdeEsquerda() ||
      detectouVerdeDireita()
    ) {

      leituraVerde++;
    }

    cont++;

    delay(10);
  }

  Serial.print("Leituras pretas: ");
  Serial.println(leituraPreto);

  Serial.print("Leituras verdes: ");
  Serial.println(leituraVerde);

  // =================================================
  // DECISÃO
  // =================================================

  if (leituraPreto > leituraVerde) {

    Serial.println("CRUZAMENTO DETECTADO");

    Cruzamento();
  }

  else {

    Serial.println("CURVA 180");

    curva180();
  }
}

// =====================================================
// FRENTE
// =====================================================

void frente(int tempo) {

  MotorEsquerdaFrente->run(BACKWARD);
  MotorEsquerdaTras->run(BACKWARD);

  MotorDireitaFrente->run(BACKWARD);
  MotorDireitaTras->run(BACKWARD);

  delay(tempo);
}

// =====================================================
// PARAR
// =====================================================

void Parar(int tempo) {

  MotorEsquerdaFrente->run(RELEASE);
  MotorEsquerdaTras->run(RELEASE);

  MotorDireitaFrente->run(RELEASE);
  MotorDireitaTras->run(RELEASE);

  delay(tempo);
}

// =====================================================
// RÉ
// =====================================================

void tras() {

  MotorEsquerdaFrente->run(FORWARD);
  MotorEsquerdaTras->run(FORWARD);

  MotorDireitaFrente->run(FORWARD);
  MotorDireitaTras->run(FORWARD);
}

// =====================================================
// CORREÇÃO PARA ESQUERDA
// =====================================================

void correcaoEsquerda() {

  MotorEsquerdaFrente->run(BACKWARD);
  MotorEsquerdaTras->run(BACKWARD);

  MotorDireitaFrente->run(RELEASE);
  MotorDireitaTras->run(RELEASE);

  delay(50);

  leituraSensorInfra();
}

// =====================================================
// CORREÇÃO PARA DIREITA
// =====================================================

void correcaoDireita() {

  MotorDireitaFrente->run(BACKWARD);
  MotorDireitaTras->run(BACKWARD);

  MotorEsquerdaFrente->run(RELEASE);
  MotorEsquerdaTras->run(RELEASE);

  delay(50);

  leituraSensorInfra();
}

// =====================================================
// TODOS BRANCOS
// =====================================================

void tresBrancos(int tempo) {

  // Reduz velocidade

  MotorEsquerdaFrente->setSpeed(velocidadeLenta);
  MotorEsquerdaTras->setSpeed(velocidadeLenta);

  MotorDireitaFrente->setSpeed(velocidadeLenta);
  MotorDireitaTras->setSpeed(velocidadeLenta);

  MotorEsquerdaFrente->run(BACKWARD);
  MotorEsquerdaTras->run(BACKWARD);

  MotorDireitaFrente->run(BACKWARD);
  MotorDireitaTras->run(BACKWARD);

  delay(tempo);

  // Restaura velocidade

  MotorEsquerdaFrente->setSpeed(velocidadeNormal);
  MotorEsquerdaTras->setSpeed(velocidadeNormal);

  MotorDireitaFrente->setSpeed(velocidadeNormal);
  MotorDireitaTras->setSpeed(velocidadeNormal);
}

// =====================================================
// CURVA PARA ESQUERDA
// =====================================================

void CurvaEsquerda() {

  Parar(150);

  // Avança até entrar na curva

  frente(800);

  // Gira para esquerda

  MotorDireitaFrente->run(BACKWARD);
  MotorDireitaTras->run(BACKWARD);

  MotorEsquerdaFrente->run(FORWARD);
  MotorEsquerdaTras->run(FORWARD);

  delay(1500);

  Parar(0);
}

// =====================================================
// CURVA PARA DIREITA
// =====================================================

void CurvaDireita() {

  Parar(150);

  // Avança até entrar na curva

  frente(800);

  // Gira para direita

  MotorDireitaFrente->run(FORWARD);
  MotorDireitaTras->run(FORWARD);

  MotorEsquerdaFrente->run(BACKWARD);
  MotorEsquerdaTras->run(BACKWARD);

  delay(1500);

  Parar(0);
}

// =====================================================
// CRUZAMENTO
// =====================================================

void Cruzamento() {

  Parar(150);

  Serial.println("ATRAVESSANDO CRUZAMENTO");

  frente(1500);

  Parar(0);
}

// =====================================================
// CURVA 180°
// =====================================================

void curva180() {

  Parar(150);

  // Avança um pouco

  frente(800);

  // Giro de 180°

  MotorDireitaFrente->run(FORWARD);
  MotorDireitaTras->run(FORWARD);

  MotorEsquerdaFrente->run(BACKWARD);
  MotorEsquerdaTras->run(BACKWARD);

  delay(2500);

  Parar(0);
}

// =====================================================
// TESTE DE GIRO
// =====================================================

void Teste() {

  MotorDireitaFrente->run(FORWARD);
  MotorDireitaTras->run(FORWARD);

  MotorEsquerdaFrente->run(BACKWARD);
  MotorEsquerdaTras->run(BACKWARD);
}
