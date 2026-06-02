#include <Arduino.h>
#include "esp_now.h"
#include "WiFi.h"
#include "esp_wifi.h"

uint32_t version;
esp_now_peer_info_t peerInfo;
uint8_t dst_mac[6] = { 0xac, 0xeb, 0xe6, 0xc1, 0xe5, 0x3c };

//estructura para datos
typedef struct{
  int a;
  float b;
  String c;
} struct_datos;

struct_datos data_esp;
struct_datos rcv_data_esp;

//establecer PMK y LMK, misma en ambos extremos
static const char* PMK_KEY_STR = "1234";
static const char* LMK_KEY_STR = "4321";

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

static void send_data_cb(const esp_now_send_info_t *tx_info, esp_now_send_status_t status)
{
    char macAddr[18];
    to_hex(macAddr, 18, tx_info->des_addr, 6);
    if(tx_info->tx_status == WIFI_SEND_SUCCESS){
      Serial.printf("Envio Correcto hacia %s \n", macAddr);
    }
    else{
      Serial.printf("Error en el envio hacia %s \n",macAddr);
    }
}


static void rcv_data_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
{
   Serial.println("Dato Recibido");
   memcpy(&rcv_data_esp, data, len);
   Serial.printf("a = %d \n", rcv_data_esp.a);
   Serial.printf("b = %f \n", rcv_data_esp.b);
   Serial.printf("c = %s \n", rcv_data_esp.c);
   Serial.flush();

}


void SendMessage()
{
	data_esp.a = 3;
	data_esp.b = 1.8;
	data_esp.c = "Hello";

	esp_err_t result = esp_now_send(peerInfo.peer_addr, (uint8_t*)&data_esp, sizeof(struct_datos));

	if(result == ESP_OK)
	{
		Serial.println("Sent with success");
	}
	else
	{
		Serial.println("Error sending the data");
	}
}

void setup() {
  // put your setup code here, to run once:

  //Inicializar Puerto Serie
  Serial.begin(115200);

  //Inicializar ESP como WiFi Station
  WiFi.mode(WIFI_STA);
  while(!WiFi.STA.started()){
    delay(100);
  }

  Serial.printf("Wifi channel is = %d \n", WiFi.channel());


  //Mostrar la MAC por el Puerto Serie
  Serial.println("MAC Address: " + WiFi.macAddress());

  //largo alcance
  ESP_ERROR_CHECK( esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR) );


  //Inicializar ESPNOW
  ESP_ERROR_CHECK(esp_now_init());
  //Obtener la version de ESPNOW por el puerto Serie
  ESP_ERROR_CHECK(esp_now_get_version(&version));
  Serial.printf("ESP-NOW version = %d", version);

  // Set PMK key
  esp_now_set_pmk((uint8_t *)PMK_KEY_STR);

  //bajo consumo por ventanas de tiempo
  //ESP_ERROR_CHECK( esp_now_set_wake_window(50) );
  //ESP_ERROR_CHECK( esp_wifi_connectionless_module_set_wake_interval(100) );

  //registrar callback de rcv y send
  ESP_ERROR_CHECK( esp_now_register_send_cb(send_data_cb) );
  ESP_ERROR_CHECK( esp_now_register_recv_cb(rcv_data_cb) );

  //add dispositivo para la comunicacion
  //copiar MAC destino al PeerInfo
  memcpy(peerInfo.peer_addr, dst_mac, 6);
  //add LMK al peerInfo
  for (uint8_t i = 0; i < 16; i++) {
    peerInfo.lmk[i] = LMK_KEY_STR[i];
  }
  // activar encriptacion en PeerInfo
  peerInfo.encrypt = true;
  //registrar PeerInfo
  ESP_ERROR_CHECK(esp_now_add_peer(&peerInfo));

   Serial.printf("Channel = %d \n", peerInfo.channel);
  Serial.printf("Encrypt = %d \n", peerInfo.encrypt);


  int8_t power;
  esp_wifi_get_max_tx_power(&power);

  Serial.printf("TX Power = %d\n", power);

  esp_now_rate_config_t config_lr;
  config_lr.ersu = true;
  config_lr.dcm = false;
  config_lr.phymode = WIFI_PHY_MODE_LR;
  config_lr.rate = WIFI_PHY_RATE_LORA_250K;
  esp_now_set_peer_rate_config(peerInfo.peer_addr, &config_lr);

}

void loop() {
  // put your main code here, to run repeatedly:
  SendMessage();

	delay(8000);
}
