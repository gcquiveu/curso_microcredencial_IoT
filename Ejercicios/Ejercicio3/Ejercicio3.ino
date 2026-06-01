#include <Arduino.h>
#include "WiFi.h"
#include "esp_now.h"

//estructura para datos
uint32_t version;
esp_now_peer_info_t peerInfo;
uint8_t dst_mac[6] = { 0xac, 0xa7, 0x04, 0xbf, 0xe8, 0xA0 };

//estructura para datos
typedef struct{
  int a;
  float b;
  char c[128];
} struct_datos;

struct_datos data_esp;
struct_datos rcv_data_esp;


unsigned long lastTime = 0;  
unsigned long timerDelay = 5000;  // send readings timer

//funcion auxiliar
bool to_hex(char* dest, size_t dest_len, const uint8_t* values, size_t val_len) {
    if(dest_len < (val_len*2+1)) /* check that dest is large enough */
        return false;
    *dest = '\0'; /* in case val_len==0 */
    while(val_len--) {
        /* sprintf directly to where dest points */
        sprintf(dest, "%02X", *values);
        dest += 2;
        ++values;
    }
    return true;
}

static void send_data_cb(const esp_now_send_info_t *tx_info, esp_now_send_status_t status){
  //comprobar el estado del envio
    char macAddr[18];
    to_hex(macAddr, 18, tx_info->des_addr, 6);
    if(tx_info->tx_status == WIFI_SEND_SUCCESS){
      Serial.printf("Envio Correcto hacia %s \n", macAddr);
      Serial.flush();
    }
    else{
      Serial.printf("Error en el envio hacia %s \n",macAddr);
    }
}


static void rcv_data_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len){
   //Imprimir por pantalla la direccion MAC del emisor
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

  //Obtener la version de ESPNOW por el puerto Serie
  ESP_ERROR_CHECK(esp_now_get_version(&version));
  Serial.printf("ESP-NOW version = %d \n", version);


  //registrar callback de rcv y send
  ESP_ERROR_CHECK( esp_now_register_send_cb(send_data_cb) );
  ESP_ERROR_CHECK( esp_now_register_recv_cb(rcv_data_cb) );

  //add dispositivo para la comunicacion
  memcpy(peerInfo.peer_addr, dst_mac, 6);
  ESP_ERROR_CHECK(esp_now_add_peer(&peerInfo));

}

void loop() {
  // put your main code here, to run repeatedly:

  if ((millis() - lastTime) > timerDelay) {
    //poner datos correctos en una instancia de la estructura para datos
    data_esp.a = 3;
    data_esp.b = 1.8;
    strcpy(data.esp.c, "Cadena de caracteres a enviar");

    //enviar por ESPNOW
    esp_err_t result = esp_now_send(peerInfo.peer_addr, (uint8_t*)&data_esp, sizeof(struct_datos));

    //esperar un tiempo
    lastTime = millis();
  }
}
