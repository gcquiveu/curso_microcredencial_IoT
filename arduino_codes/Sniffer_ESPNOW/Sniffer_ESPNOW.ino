#include "esp_wifi.h"

// Configura el canal en el que operan tus dispositivos ESP-NOW
#define WIFI_CHANNEL 1 

void sniffer_callback(void* buf, wifi_promiscuous_pkt_type_t type) {
  // Solo nos interesan paquetes de datos (donde viaja ESP-NOW)
  if (type != WIFI_PKT_DATA) return;

  wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t*)buf;
  uint8_t *payload = pkt->payload;
  uint16_t len = pkt->rx_ctrl.sig_len;

  // Verificación rápida: Un paquete ESP-NOW válido mide al menos 40 bytes
  // y tiene el identificador OUI de Espressif (00:18:fe) en la posición del Vendor Specific
  if (len < 40) return;
  
  // Buscamos el identificador de Espressif en el frame de datos
  bool is_espnow = false;
  for (int i = 0; i < len - 3; i++) {
    if (payload[i] == 0x00 && payload[i+1] == 0x18 && payload[i+2] == 0xFE) {
      is_espnow = true;
      break;
    }
  }

  // Si no es un paquete ESP-NOW, ignoramos el resto del tráfico Wi-Fi del aire
  if (!is_espnow) return;

  // Extraer MACs (Encabezado MAC estándar 802.11)
  // payload[4..9] -> MAC Destino | payload[10..15] -> MAC Origen
  Serial.printf("\n--- ¡Paquete ESP-NOW Detectado! (%d bytes) ---\n", len);
  
  Serial.print("DES: ");
  for(int i=4; i<10; i++) Serial.printf("%02X%s", payload[i], (i==9)?"":":");
  
  Serial.print(" | ORG: ");
  for(int i=10; i<16; i++) Serial.printf("%02X%s", payload[i], (i==15)?"\n":":");

  // Mostrar el contenido crudo en HEX
  Serial.print("Datos (HEX): ");
  for (int i = 0; i < len; i++) {
    Serial.printf("%02X ", payload[i]);
    if ((i + 1) % 16 == 0) Serial.print("\n             "); // Formato limpio cada 16 bytes
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Iniciando Sniffer Simple de ESP-NOW...");

  // Inicializar Wi-Fi en modo Promiscuo
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL); // No se conecta a ningún AP
  esp_wifi_start();

  // Fijar canal y activar escucha
  esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous_rx_cb(&sniffer_callback);
  esp_wifi_set_promiscuous(true);

  Serial.printf("Escuchando activamente en el Canal %d...\n", WIFI_CHANNEL);
}

void loop() {
  // Todo se gestiona por interrupciones en el callback
  delay(100);
}