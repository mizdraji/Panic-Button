#include "Arduino.h"
//rta_type
#define espera_ACK 0
#define espera_Dato 1
#define no_espera_ACK 2

//dato entrante
#define INPUTBUFF 255
char datoEntrante[INPUTBUFF] = {0};
char cmd_rcv;
byte recvStatus = 0;                  //no usar bool para esta bandera ya que genera multiples interrupciones.

uint8_t initLoraTec();
uint8_t sendPackage( char *data_to_send, uint8_t len, uint8_t rta_type,  bool canal_por_defecto );