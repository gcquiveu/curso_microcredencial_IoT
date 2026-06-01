#include <Arduino.h>
#include "WiFi.h"
#include "esp_now.h"


esp_now_peer_info_t peerInfo;
uint8_t dst_mac[6] = { 0xac, 0xa7, 0x04, 0xbf, 0xe8, 0xA0 };

//estructura para datos
typedef struct{
  int a;
  float b;
  String c;
} struct_datos;

struct_datos data_esp;
struct_datos rcv_data_esp;

static void send_data_cb(const esp_now_send_info_t *tx_info, esp_now_send_status_t status){
  //comprobar el estado del envio
  Serial.println("Enviado");

}


static void rcv_data_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len){
   //imprimir por el puerto Serie los datos recibidos
   Serial.println("Dato Recibido");
   memcpy(&rcv_data_esp, data, len);
   Serial.printf("a = %d \n", rcv_data_esp.a);
   Serial.printf("b = %f \n", rcv_data_esp.b);
   Serial.printf("c = %s \n", rcv_data_esp.c);
   Serial.flush();
}

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

  //Inicializar ESPNOW
  ESP_ERROR_CHECK(esp_now_init());

  //registrar callback de rcv y send
  ESP_ERROR_CHECK( esp_now_register_send_cb(send_data_cb) );
  ESP_ERROR_CHECK( esp_now_register_recv_cb(rcv_data_cb) );

  //add dispositivo para la comunicacion
  memcpy(peerInfo.peer_addr, dst_mac, 6);
  ESP_ERROR_CHECK(esp_now_add_peer(&peerInfo));

}

void loop() {
  // put your main code here, to run repeatedly:

  //poner datos correctos en una instancia de la estructura para datos
	data_esp.a = 3;
	data_esp.b = 1.8;
	data_esp.c = "Hello";

  //enviar por ESPNOW
	esp_err_t result = esp_now_send(peerInfo.peer_addr, (uint8_t*)&data_esp, sizeof(struct_datos));

  //esperar un tiempo
  delay(5000);
}
