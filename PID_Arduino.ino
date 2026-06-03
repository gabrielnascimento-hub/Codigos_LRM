#include <PID_v1.h>
#include <Adafruit_MotorShield.h>

double Input, Output, Setpoint = 0;

double Kp = 0, Ki = 2, Kd = 0;
PID meuPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

int velBase = 150;  


Adafruit_MotorShield AFMS = Adafruit_MotorShield();


  Adafruit_DCMotor *MotorEsquerdaFrente = AFMS.getMotor(1); //motor esq frente
  Adafruit_DCMotor *MotorEsquerdaTras = AFMS.getMotor(2); //motor esq tras
  Adafruit_DCMotor *MotorDireitaFrente = AFMS.getMotor(3); //motor direita frente
  Adafruit_DCMotor *MotorDireitaTras = AFMS.getMotor(4); //motor direita tras

void setup() {
  AFMS.begin();
  meuPID.SetMode(AUTOMATIC);
  meuPID.SetOutputLimits(-100,100);
  meuPID.SetSampleTime(10);
  MotorEsquerdaFrente->run(FORWARD);
  MotorEsquerdaTras->run(FORWARD);
  MotorDireitaFrente->run(FORWARD);
  MotorDireitaTras->run(FORWARD);
  leituraEsq = analogRead(AnalogicoEsquerda);

}

void loop() {
  // 1. LEITURA DOS SENSORES (Exemplo simples usando os analógicos)
  // Se o sensor esquerdo lê mais que o direito, o erro é negativo. Se o direito lê mais, é positivo.

  
  Input = leituraDir - leituraEsq; // O erro que o PID vai tentar zerar

  // 2. CÁLCULO PID
  meuPID.Compute();

  // 3. APLICAÇÃO NOS MOTORES
  // O 'Output' vai de -100 a 100 (conforme definido no SetOutputLimits)
  int velEsq = velBase + Output;
  int velDir = velBase - Output;

  // Garante que a velocidade fique dentro do limite do Motor Shield (0-255)
  velEsq = constrain(velEsq, 0, 255);
  velDir = constrain(velDir, 0, 255);

  // Aplica as velocidades nos 4 motores
  MotorEsquerdaFrente->setSpeed(velEsq);
  MotorEsquerdaTras->setSpeed(velEsq);
  MotorDireitaFrente->setSpeed(velDir);
  MotorDireitaTras->setSpeed(velDir);

}
