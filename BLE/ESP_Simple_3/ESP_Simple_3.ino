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
#include <BLE2904.h> 

















// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

// La UUID que le vamos a asignar al dispositivo
#define SERVICE_UUID            "4fafc201-1fb5-459e-8fcc-c5c9c331914b"

// UUIDs de los servicios.
#define GENERIC_ACCESS_UUID     "00001800-0000-1000-8000-00805f9b34fb"
#define DEVICE_INFORMATION_UUID "0000180a-0000-1000-8000-00805f9b34fb"
#define BATTERY_SERVICE_UUID    "0000180f-0000-1000-8000-00805f9b34fb"

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

// Característica Battery Level
BLECharacteristic BatteryLevelCharacteristic(
  BLEUUID((uint16_t)0x2A19),
  BLECharacteristic::PROPERTY_READ | 
  BLECharacteristic::PROPERTY_NOTIFY
  );

// Servidor de BLE
BLEServer  *pServer;

// Servicio General Access
BLEService *pGeneralAccess;

// Servicio Device Information
BLEService *pDeviceInfo;

// Servicio de Battery
BLEService *pBatteryService;

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
  static uint8_t level = 50;
  // Formato usado (hay otros): setValue(uint8_t *, size)
  BatteryLevelCharacteristic.setValue(&level, 1);
  // Se activa la notificación de nuevo valor.
  BatteryLevelCharacteristic.notify(true);
   // Se manda la info por puerto serie (no necesario)
  Serial.print(int(level));
  Serial.print(" ");


  // Actualizamos las variables.
  level++;
  if (int(level) == 100)
  {
    level = 0;
  }
  
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
  DeviceNameCharacteristic.setValue(ble_name);
  
  // Crea el servicio Device Name
  pDeviceInfo = pServer->createService(DEVICE_INFORMATION_UUID);
  // Añade la característica Manufacturer Name String
  pDeviceInfo->addCharacteristic(&ManufacturerNameCharacteristic);
  // Añade el nombre del fabricante
  ManufacturerNameCharacteristic.setValue("Departamento de Tecnología Electrónica");

  // Crea el servicio de Battery
  pBatteryService = pServer->createService(BATTERY_SERVICE_UUID);
  // Añade la característica del nivel de batería.
  pBatteryService->addCharacteristic(&BatteryLevelCharacteristic);
  // Añade las notificaciones.
  BatteryLevelCharacteristic.addDescriptor(new BLE2902());
  // Condiguración de un descriptor de formato
  BLE2904* batteryLevelDescriptor = new BLE2904();
  batteryLevelDescriptor->setFormat(BLE2904::FORMAT_UINT8);
  batteryLevelDescriptor->setNamespace(1);
  // Esta UUID indica que es un porcentaje.
  batteryLevelDescriptor->setUnit(0x27ad);
  // Añade el descriptor.
  BatteryLevelCharacteristic.addDescriptor(batteryLevelDescriptor);
  

  // Añade el servicio Generic Access
  pServer->getAdvertising()->addServiceUUID(GENERIC_ACCESS_UUID);
  // Añade el servicio Device Information
  pServer->getAdvertising()->addServiceUUID(DEVICE_INFORMATION_UUID);
  // Añade el servicio Battery
  pServer->getAdvertising()->addServiceUUID(BATTERY_SERVICE_UUID);
  

  // Inicia los servicios
  pGeneralAccess->start();
  pDeviceInfo->start();
  pBatteryService->start();
  
  // Inicia la publicación.
  pServer->getAdvertising()->start();
}
