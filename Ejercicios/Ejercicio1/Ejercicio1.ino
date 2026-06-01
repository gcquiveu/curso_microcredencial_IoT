#include <Arduino.h>
#include "WiFi.h"

void setup() {
  // put your setup code here, to run once:
  //Inicializar el Puerto Serie
  Serial.begin(115200);

  //Inicializar ESP como WiFi Station
  WiFi.mode(WIFI_STA);
  while(!WiFi.STA.started()){
    delay(100);
  }

  //Mostrar la MAC por el puerto Serie
  Serial.println("MAC Address: " + WiFi.macAddress());
}

void loop() {
  // put your main code here, to run repeatedly:

}
