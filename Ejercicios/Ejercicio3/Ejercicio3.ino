#include <Arduino.h>
#include "WiFi.h"
#include "esp_now.h"

//estructura para datos


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
}


static void rcv_data_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len){
   //Imprimir por pantalla la direccion MAC del emisor
   //imprimir por el puerto Serie los datos recibidos
}

void setup() {
  // put your setup code here, to run once:
  //Inicializar el Puerto Serie

  //Inicializar ESP como WiFi Station

  //Mostrar la MAC por el puerto Serie

  //Inicializar ESPNOW

  //Obtener la version de ESPNOW por el puerto Serie


  //registrar callback de rcv y send

  //add dispositivo para la comunicacion

}

void loop() {
  // put your main code here, to run repeatedly:

  //poner datos correctos en una instancia de la estructura para datos

  //enviar por ESPNOW

  //esperar un tiempo
}