// O Objetivo deste código é verificar, de maneira simples e superficial, o funcionamento dos pares
//resistores-termistores das Smart Sleeves.


#include <evolver_si.h>
#include <Tlc5940.h>

int s0 = 7, s1 = 8, s2 = 9, s3 = 10, SIG_PIN = 0; //define pinos digitais 7-10 do arduino como controles do MUX e analógico 0 como inpt de valores.

void setup(){

SerialUSB.begin(9600);//começa serial USB (comunicação com o monitor serial)
Serial1.begin(9600);
pinMode(12, OUTPUT);
  
  digitalWrite(12, LOW);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);

  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);

  analogReadResolution(12);

}
