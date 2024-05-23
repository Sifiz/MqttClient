#include <Arduino.h>
#include <CLight_sensor.h>

// put function declarations here:
int led = 32;
int photo_transistor = 34;
int brightness = 0;
int pwmChannel = 0; //Choisit le canal 0
int frequence = 10000; //Fréquence PWM de 1 KHz
int resolution = 8; // Résolution de 8 bits, 256 valeurs possibles
int pwmPin = 23;
int T = 350;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(photo_transistor, INPUT);
  Serial.println("Test project starting...");
  // Configuration du canal 0 avec la fréquence et la résolution choisie
  ledcSetup(pwmChannel, frequence, resolution);
  // Assigne le canal PWM au pin 23
  ledcAttachPin(pwmPin, pwmChannel);
  // Créer la tension en sortie choisi
  ledcWrite(pwmChannel, 127); //1.65 V
}

void loop() {
  // put your main code here, to run repeatedly:
  int lux = analogRead(photo_transistor);
  //change frequency of pmw
  ledcWriteTone(pwmChannel, map(lux, 0, 255, 0, 4095)); // update the PWM frequency
}

