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

// UUIDs de los servicios.
#define GENERIC_ACCESS_UUID     "00001800-0000-1000-8000-00805f9b34fb"
#define DEVICE_INFORMATION_UUID "0000180a-0000-1000-8000-00805f9b34fb"

// Característica Device Name
BLECharacteristic DeviceNameCharacteristic(
  BLEUUID((uint16_t)0x2A00),
  BLECharacteristic::PROPERTY_READ | 
  BLECharacteristic::PROPERTY_WRITE
  );

// Caracterísitca Manufacturer Name String
BLECharacteristic ManufacturerNameCharacteristic(
  BLEUUID((uint16_t)0x2A29),
  BLECharacteristic::PROPERTY_READ
);

// Servidor de BLE
BLEServer  *pServer;

// Servicio General Access
BLEService *pGeneralAccess;

// Servicio Device Information
BLEService *pDeviceInfo;

// Característica Device Name
BLECharacteristic *pDeviceName;

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
  Serial.println("ok");
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
  
  // Crea el servicio Generic Access
  pGeneralAccess = pServer->createService(GENERIC_ACCESS_UUID);
  // Añade la característica del Generic Access
  pGeneralAccess->addCharacteristic(&DeviceNameCharacteristic);
  // Cambia el nombre - similar a  BLEDevice::init(ble_name), si no
  // sustituyesemos la característica.
  DeviceNameCharacteristic.setValue("ESP32-2");
  
  // Crea el servicio Device Name
  pDeviceInfo = pServer->createService(DEVICE_INFORMATION_UUID);
  // Añade la característica Manufacturer Name String
  pDeviceInfo->addCharacteristic(&ManufacturerNameCharacteristic);
  // Añade el nombre del fabricante
  ManufacturerNameCharacteristic.setValue("Departamento de Tecnología Electrónica");

  // Añade el servicio Generic Access
  pServer->getAdvertising()->addServiceUUID(GENERIC_ACCESS_UUID);
  // Añade el servicio Device Information
  pServer->getAdvertising()->addServiceUUID(DEVICE_INFORMATION_UUID);

  // Inicia los servicios
  pGeneralAccess->start();
  pDeviceInfo->start();
  
  // Inicia la publicación.
  pServer->getAdvertising()->start();
}
