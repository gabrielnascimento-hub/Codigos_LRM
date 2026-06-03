#include <PID_v1.h>

double Input, Output, Setpoint;

double Kp = 0, Ki = 0, Kd = 0;

PID meuPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

int velBase = 150;

void setup() {
  Setpoint = 100; // Valor que queremos alcançar
  myPID.SetMode(AUTOMATIC);
}

void loop() {
  Input = analogRead(sensorPin); // Lê o sensor atual
  myPID.Compute();               // Calcula a correção necessária
  analogWrite(atuadorPin, Output);

}
