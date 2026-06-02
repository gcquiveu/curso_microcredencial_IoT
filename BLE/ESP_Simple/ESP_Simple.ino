/*****
 * Condiguración básica de la ESP32 para el curso:
 * Microcredencial Introducción al IoT con ESP32 y ESP8266 (I Edición)
 * @Author: Juan A. Castro-Garcia
 * @Date: 2026-06-01
 * @Version: 1.0
 *****/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h> 

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

// La UUID que le vamos a asignar al dispositivo
#define SERVICE_UUID            "4fafc201-1fb5-459e-8fcc-c5c9c331914b"

// Servidor de BLE
BLEServer  *pServer;

// Nombre que le vamos a asignar inicalmente al dispositivo.
String ble_name = "ESP32-1";

void setup() {
  // Configuración del puerto serie.
  Serial.begin(115200);
  
  Serial.print("Salida de datos del dispositivo ");
  Serial.print(ble_name.c_str());
  Serial.println(":");

  // Configuración del BLE
  InitBLE();
}

void loop() {
  // Espera de 1 segundo.
  delay(1000);
}


/*****
 *  Función que configura el BLE
 *****/
void InitBLE() {
  // Inicia el BLE y le asigna el nombre.
  BLEDevice::init(ble_name);
  
  // Crea el BLE Server.
  pServer = BLEDevice::createServer();
  
  // Inicia la publicación.
  pServer->getAdvertising()->start();
}
