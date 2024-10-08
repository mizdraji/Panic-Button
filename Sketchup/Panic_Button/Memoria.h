//  Manejo de memoria EEPROM (Flash)
#include <EEPROM.h>
//#include "payloads.h" 

#define EEPROM_SIZE 100

#define SLOTS_CANTIDAD_MAX 16 //Cantidad maxima de payloads que se pueden almacenar     ///, no creo que use esto

//byte cantSlot;       //Cantidad de slots guardados en memoria


//Direcciones de memoria
//#define CMT_CANTIDAD_MAX 100 //Cantidad maxima de marcas de tiempo que se pueden almacenar
int pos_mem[SLOTS_CANTIDAD_MAX]={0};    //Array donde se guardan las posiciones de memoria de los slots almacenados
//const int pos_modo_wifi=(SLOTS_CANTIDAD_MAX*(PL_CONT_MONO_SIZE+2))+(CMT_CANTIDAD_MAX*5); //2724 direccion del byte para conectar o no al wifi al inicio (1 byte)
//const int pos_bit_act=; //2725 bit de activacion (1 byte) 
#define pos_bit_act 0       //bit de activacion (1 byte)
//const int pos_devID=pos_bit_act+1;	//2726 devID (4 + 1 bytes) 
#define pos_devID 1         //devID (5 bytes)
//const int pos_devAddr=pos_devID+5;	//2731	(8+1 bytes)
#define pos_devAddr 55      // 8 +1 bytes
#define pos_D1 6            //Destinatario1  (10 bytes)
#define pos_D2 16           //Destinatario2  (10 bytes)
#define pos_D3 26           //Destinatario3  (10 bytes)
#define pos_FV 36           //Firmware Version (4 bytes)
#define pos_HV 40           //Hardware version (4 bytes)
#define pos_MN 44           //Mi número, número de teléfono del módulo  (10 bytes)
#define pos_reset_count 54  //contador de reseteos (1 byte)

byte slot_actual=0;	//Numero de bloque de memoria (0-15) donde se almacenan las payloads
int dir_actual=0;	//Dirección de memoria donde se almacena la payload actual
int dir_CMt_actual=0;	//Direccion de memoria donde se almacena la marca de tiempo
unsigned long CMt_corte=0; //Ultima marca de tiempo registrada antes de que se reinicie el dispositivo
bool cambio_bloque_PL=1; //0 si no cambio la memoria en el bloque de payloads, 1 si cambio algo
byte slot_para_enviar=0; //Numero de bloque de memoria (0-15) donde se almacena el paquete proximo a ser enviado









void initEEPROM();
void updateEEPROM(int addr, byte dato_in);
void clearEEPROM();
void writeResetCountToEEPROM(byte reset_count_);
byte readCredFromEEPROM();